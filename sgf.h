//****************************************************************************
//
// last updated: 2006-08-06
// Contents
// 
// SGF basics
// Basic (EBNF) definition
// EBNF definition
// Some remarks about properties
// Property types
// Property attributes
// How to handle unknown/faulty properties
// Private Properties
// Property Value Types
// Double
// Text
// SimpleText
// Stone
// Move / Point
// Compressed point lists
//
// 1. SGF basics
// 
// SGF is a text-only format (not a binary format).

// It contains game trees, with all their nodes and properties, and
// nothing more. Thus the file format reflects the regular internal
// structure of a tree of property lists. There are no exceptions; if
// a game needs to store some information on file with the document, a
// (game-specific) property must be defined for that purpose.
//
// This tree is written in pre-order as: (root(ab(c)(de))(f(ghi)(j)))
//
// SGF example:
//
// (;FF[4]C[root](;C[a];C[b](;C[c])
// (;C[d];C[e]))
// (;C[f](;C[g];C[h];C[i])
// (;C[j])))
//
// Example for tree structure.
//
//        [root]
//       /      \
//    [a]        [f]
//     |         / \
//    [b]     [g]   [j]
//    / \      |
// [c]   [d]  [h]
//        |    |
//       [e]  [i]
//
// Tree as seen by the user.
//
// [root]--[B1]a--[W2]b--[B3]c
//   \               \
//    \               +--[B3]d--[W4]e
//     \
//      +--[B1]f--[W2]g--[B3]h--[W4]i
//            \
//             +--[W2]j
//
// The first line is the main line of play, the other lines are
// variations.
//
// There are more examples available.
//
//****************************************************************************
// 
// Node numbering:
//
// When numbering nodes starting with zero is suggested. Nodes should
// be numbered in the way they are stored in the file.
//
// Example (of file above): root=0, a=1, b=2, c=3, d=4, e=5, f=6, g=7,
// h=8, i=9 and j=10.
// 
// SGF uses the US ASCII char-set for all its property identifiers and
// property values, except SimpleText & Text. For SimpleText & Text
// the charset is defined using the CA property.
// 
// 2. Basic (EBNF) Definition
// 
// The conventions of EBNF are discussed in literature, and on WWW in
// the Computing Dictionary.  A quick summary:
// 
//   "..." : terminal symbols
//   [...] : option: occurs at most once
//   {...} : repetition: any number of times, including zero
//   (...) : grouping
//     |   : exclusive or
//  italics: parameter explained at some other place
//  
// 2.1. EBNF Definition
// 
//   Collection = GameTree { GameTree }
//   GameTree   = "(" Sequence { GameTree } ")"
//   Sequence   = Node { Node }
//   Node       = ";" { Property }
//   Property   = PropIdent PropValue { PropValue }
//   PropIdent  = UcLetter { UcLetter }
//   PropValue  = "[" CValueType "]"
//   CValueType = (ValueType | Compose)
//   ValueType  = (None | Number | Real | Double | Color | SimpleText |
// 		Text | Point  | Move | Stone)
//
// White space (space, tab, carriage return, line feed, vertical tab
// and so on) may appear anywhere between PropValues, Properties,
// Nodes, Sequences and GameTrees.
//
// There are two types of property lists: 'list of' and 'elist of'.
// 
// 'list of':    PropValue { PropValue }
// 'elist of':   ((PropValue { PropValue }) | None)
//               In other words elist is list or "[]".
// 
// 2.2. Some remarks about properties
// 
// Property-identifiers are defined as keywords using only uppercase
// letters. Currently there are no more than two uppercase letters per
// identifier.
//
// The order of properties in a node is not fixed. It may change every
// time the file is saved and may vary from application to
// application. Furthermore applications should not rely on the order
// of property values. The order of values might change as well.
// 
// Everybody is free to define additional, private properties, as long
// as they do not interfere with the standard properties defined in
// this document.
// 
// Therefore, if one is writing a SGF reader, it is important to skip
// unknown properties. An application should issue a warning message
// when skipping unknown or faulty properties.
// 
// Only one of each property is allowed per node, e.g. one cannot have
// two comments in one node:
// 
// ... ;  C[comment1]  B  [dg]  C[comment2] ; ...
//
// This is an error.
//
// Each property has a property type. Property types place
// restrictions on certain properties e.g. in which nodes they are
// allowed and with which properties they may be combined.
// 
// 2.2.1. Property Types (currently five):
// 
// move Properties of this type concentrate on the move made, not on
// 	the position arrived at by this move.
//
// 	Move properties must not be mixed with setup properties within
// 	the same node.
//
// 	Note: it's bad style to have move properties in root nodes.
// 	(it isn't forbidden though)
// 
// setup Properties of this type concentrate on the current position.
// 	Setup properties must not be mixed with move properties within
// 	a node.
// 
// root Root properties may only appear in root nodes. Root nodes are
// 	the first nodes of gametrees, which are direct descendants
// 	from a collection (i.e. not gametrees within other gametrees).
// 	They define some global 'attributes' such as board-size, kind
// 	of game, used file format etc.
// 
// game-info Game-info properties provide some information about the
// 	game played (e.g. who, where, when, what, result, rules,
// 	etc.).  These properties are usually stored in root nodes.
// 	When merging a set of games into a single gametree, game infos
// 	are stored at the nodes where a game first becomes
// 	distinguishable from all other games in the tree.
// 
//      A node containing game-info properties is called a game-info
// 	node.  There may be only one game-info node on any path within
// 	the tree, i.e. if some game-info properties occur in one node
// 	there may not be any further game-info properties in following
// 	nodes:
//
//      a) on the path from the root node to this node.
//
//      b) in the subtree below this node.
// 
// -    no type. These properties have no special types and may appear
// 	anywhere in a collection.
//
// Because of the strict distinction between move and setup properties
// nodes could also be divided into two classes: move-nodes and
// setup-nodes. This is important for databases, converting to/from
// ISHI-format and for some special applications.
//
// 2.2.2. Property attributes (currently only one)
// 
// inherit Properties having this attribute affect not only the node
// 	containing these properties but also ALL subsequent child
// 	nodes as well until a new setting is encountered or the
// 	setting gets cleared.  I.e. once set all children (of this
// 	node) inherit the values of the 'inherit' type properties.
//
// 	E.g. VW restricts the view not only of the current node, but
// 	of all successors nodes as well. Thus a VW at the beginning of
// 	a variation is valid for the whole variation tree.
//
// 	Inheritance stops, if either a new property is encountered and
// 	those values are inherited from now on, or the property value
// 	gets cleared, typically by an empty value, e.g. VW[].
//
// 2.2.3. How to handle unknown/faulty properties
// 
// Unknown properties and their values should be preserved.
//
// If an application isn't able to preserve unknown properties, then
// it has to display a warning message.  Illegally formatted game-info
// properties should be corrected if possible, otherwise preserved.
// Other illegally formatted properties should be corrected if
// possible, otherwise deleted.  An application has to display a
// warning message, if it deletes illegally formatted properties.
//
// 2.2.4. Private Properties
// 
// Applications may define their own private properties. Some
// restrictions apply.
//
// Property identifier: private properties must not use an identifier
// used by one of the standard properties. You have to use a new
// identifier instead. The identifier should consist of up to two
// uppercase letters. SGF doesn't require to limit the identifier to
// two letters, but some applications could break otherwise.
// 
// Property value: private properties may use one of the value types
// defined in this document or define their own value type. When using
// a private value type the application has to escape every "]" with a
// leading "\". Otherwise the file would become unparseable. Should
// the value type be a combination of two simpler types then it's
// suggested that your application uses the Compose type.
// 
// 3. Property Value Types
// 
//   UcLetter   = "A".."Z"
//   Digit      = "0".."9"
//   None       = ""
// 
//   Number     = [("+"|"-")] Digit { Digit }
//   Real       = Number ["." Digit { Digit }]
// 
//   Double     = ("1" | "2")
//   Color      = ("B" | "W")
// 
//   SimpleText = { any character (handling see below) }
//   Text       = { any character (handling see below) }
// 
//   Point      = game-specific
//   Move       = game-specific
//   Stone      = game-specific
// 
//   Compose    = ValueType ":" ValueType
//
// 3.1. Double
// 
// Double values are used for annotation properties. They are called
// Double because the value is either simple or emphasized. A value of
// '1' means 'normal'; '2' means that it is emphasized.
//
// Example: 
//
// GB[1] could be displayed as: Good for black 
//
// GB[2] could be displayed as: Very good for black
//
// 3.2. Text
// 
// Text is a formatted text. White spaces other than linebreaks are
// converted to space (e.g. no tab, vertical tab, ..).
//
// Formatting:
//
// Soft line break: linebreaks preceded by a "\" (soft linebreaks are
// converted to "", i.e. they are removed)
//
// Hard line breaks: any other linebreaks encountered
// 
// Attention: a single linebreak is represented differently on
// different systems, e.g. "LFCR" for DOS, "LF" on Unix. An
// application should be able to deal with following linebreaks: LF,
// CR, LFCR, CRLF.
//
// Applications must be able to handle Texts of any size. The text
// should be displayed the way it is, though long lines may be
// word-wrapped, if they don't fit the display.
// 
// Escaping: "\" is the escape character. Any char following "\" is
// inserted verbatim (exception: whitespaces still have to be
// converted to space!). Following chars have to be escaped, when used
// in Text: "]", "\" and ":" (only if used in compose data type).
// 
// Encoding: texts can be encoded in different charsets. See CA property.
// 
// 3.2.1. Example:
// 
// C[Meijin NR: yeah, k4 is won\
// derful
// sweat NR: thank you! :\)
// dada NR: yup. I like this move too. It's a move only to be expected from a pro. I really like it :)
// jansteen 4d: Can anyone\
//  explain [me\] k4?]
// could be rendered as:
// Meijin NR: yeah, k4 is wonderful
// sweat NR: thank you! :)
// dada NR: yup. I like this move too. It's a move only to be expected
// from a pro. I really like it :)
// jansteen 4d: Can anyone explain [me] k4?
//
// 3.3. SimpleText
// 
// SimpleText is a simple string. Whitespaces other than space must be
// converted to space, i.e. there's no newline! Applications must be
// able to handle SimpleTexts of any size.
//
// Formatting: linebreaks preceded by a "\" are converted to "",
// i.e. they are removed (same as Text type). All other linebreaks are
// converted to space (no newline on display!!).
// 
// Escaping (same as Text type): "\" is the escape character. Any char
// following "\" is inserted verbatim (exception: whitespaces still
// have to be converted to space!). Following chars have to be
// escaped, when used in SimpleText: "]", "\" and ":" (only if used in
// compose data type).
// 
// Encoding (same as Text type): SimpleTexts can be encoded in
// different charsets. See CA property.
// 
// 3.4. Stone
// 
// This type is used to specify the point and the piece that should be
// placed at that point. If a game doesn't have a distinguishable set
// of pieces (figures) like e.g. Go (GM[1]) the Stone type is reduced
// to the Point type below, e.g. "list of stone" becomes "list of
// point" for that game.
//
// Note: if a property allows "list of stone" a reduction to "list of
// point" allows compressed point lists.
//
// Go, Othello, Gomuku, Renju: Stone becomes Point
// Chess
// Nine Men's Morris
// Chinese chess
// Shogi
// Backgammon, Lines of Action, Hex, Gess: Stone becomes Point
// Amazons
// Octi
//
// 3.5. Move / Point
// 
// These two types are game specific.
// Go
// Othello
// Chess
// Gomoku, Renju
// Nine Men's Morris
// Backgammon
// Chinese chess
// Shogi
// Lines of Action
// Hex
// Amazons
// Gess
// Octi
//
// 3.5.1. Compressed point lists
// 
// The PropValue type "list of point" or "elist of point" may be compressed.
//
// Compressing is done by specifying rectangles instead of listing
// every single point in the rectangle. Rectangles are specified by
// using the upper left and lower right corner of the rectangle.
//
// Definition:
//
// List of point: list of (point | composition of point ":" point)
//
// For the composed type the first point specifies the upper left
// corner, the second point the lower right corner of the rectangle.
//
// 1x1 Rectangles are illegal - they've to be listed as single point.
//
// The definition of 'point list' allows both single point [xy] and
// rectangle [ul:lr] specifiers in any order and combination. However
// the points have to be unique, i.e. overlap and duplication are
// forbidden.
//
// To get an idea have a look at an example.
//
//****************************************************************************
//
// Go (GM[1])
// 
// Move, Point & Stone type
// 
// In Go the Stone becomes Point and the Move and Point type are the
// same: two lowercase letters.
//
// Coordinate system for points and moves
// 
// The first letter designates the column (left to right), the second
// the row (top to bottom). The upper left part of the board is used
// for smaller boards, e.g. letters "a"-"m" for 13*13.  A pass move is
// shown as '[]' or alternatively as '[tt]' (only for boards <=
// 19x19), i.e. applications should be able to deal with both
// representations. '[tt]' is kept for compatibility with FF[3].
// Using lowercase letters only the maximum board size is 26x26.
// 
// In FF[4] it is possible to specify board sizes upto 52x52. In this
// case uppercase letters are used to represent points from 27-52,
// i.e. 'a'=1 ... 'z'=26 , 'A'=27 ... 'Z'=52
// 
// How to execute a move
// 
// When a B (resp. W) property is encountered, a stone of that color
// is placed on the given position (no matter what was there before).
//
// Then the application should check any W (resp. B) groups that are
// adjacent to the stone just placed. If they have no liberties they
// should be removed and the prisoner count increased accordingly.
//
// Lastly, the B (resp. W) group that the newest stone belongs to
// should be checked for liberties, and if it has no liberties, it
// should be removed (suicide) and the prisoner count increased
// accordingly.
//
// See also: extensive explanation with examples (should leave no
// question unanswered; is part of FF5 discussion but valid for FF4 as
// well).
// 
// Properties
// 
// TW and TB points must be unique, i.e. it's illegal to list the same
// point in TB and TW within the same node.
//
// Property:	HA
// Propvalue:	number
// Propertytype: game-info
// Function:	Defines the number of handicap stones (>=2).
// 		If there is a handicap, the position should be set up
// 		with AB within the same node.
// 		HA itself doesn't add any stones to the board, nor
// 		does it imply any particular way of placing the
// 		handicap stones.
// Related:	KM, RE, RU
// 
// Property:	KM
// Propvalue:	real
// Propertytype: game-info
// Function:	Defines the komi.
// Related:	HA, RE, RU
// 
// Property:	TB
// Propvalue:	elist of point
// Propertytype: -
// Function:	Specifies the black territory or area (depends on
// 		rule set used).
// 		Points must be unique.
// Related:	TW
// 
// Property:	TW
// Propvalue:	elist of point
// Propertytype:	-
// Function:	Specifies the white territory or area (depends on
// 		rule set used).
// 		Points must be unique.
// Related:	TB
//
//****************************************************************************
//
//  last updated: 2006-06-25
// Contents
// 
// Move Properties		B, KO, MN, W
// Setup Properties		AB, AE, AW, PL
// Node Annotation Properties	C, DM, GB, GW, HO, N, UC, V
// Move Annotation Properties	BM, DO, IT, TE
// Markup Properties		AR, CR, DD, LB, LN, MA, SL, SQ, TR
// Root Properties		AP, CA, FF, GM, ST, SZ
// Game Info Properties		AN, BR, BT, CP, DT, EV, GN, GC, ON, OT, PB,
// 				PC, PW, RE, RO, RU, SO, TM, US, WR, WT
// Timing Properties		BL, OB, OW, WL
// Miscellaneous Properties	FG, PM, VW
// SGF Properties (FF[4])
//============================================================================
// Move properties
//----------------------------------------------------------------------------
// Property: B
// Propvalue: move
// Propertytype: move
// Function: Execute a black move. This is one of the most used
//     properties in actual collections. As long as the given move is
//     syntactically correct it should be executed.
//     It doesn't matter if the move itself is illegal
//     (e.g. recapturing a ko in a Go game).
//     Have a look at how to execute a Go-move.
//     B and W properties must not be mixed within a node.
// Related: W, KO
// 
//----------------------------------------------------------------------------
// Property: KO
// Propvalue: none
// Propertytype: move

// Function: Execute a given move (B or W) even it's illegal. This is
//     an optional property, SGF viewers themselves should execute ALL
//     moves. It's purpose is to make it easier for other applications
//     (e.g. computer-players) to deal with illegal moves. A KO
//     property without a black or white move within the same node is
//     illegal.
// Related: W, B
// 
//----------------------------------------------------------------------------
// Property: MN
// Propvalue: number
// Propertytype: move
// Function: Sets the move number to the given value, i.e. a move
//     specified in this node has exactly this move-number. This can
//     be useful for variations or printing.
// Related: B, W, FG, PM
// 
//----------------------------------------------------------------------------
// Property: W
// Propvalue: move
// Propertytype: move
// Function: Execute a white move. This is one of the most used
//     properties in actual collections. As long as the given move is
//     syntactically correct it should be executed.  It doesn't matter
//     if the move itself is illegal (e.g. recapturing a ko in a Go
//     game).
//     Have a look at how to execute a Go-move.
//     B and W properties must not be mixed within a node.
// Related: B, KO
//
//============================================================================
// Setup properties
// 
// Restrictions
// 
// AB, AW and AE must have unique points, i.e. it is illegal to place
// different colors on the same point within one node.
//
// AB, AW and AE values which don't change the board, e.g. placing a
// black stone with AB[] over a black stone that's already there, is
// bad style. Applications may want to delete these values and issue a
// warning.
//
//----------------------------------------------------------------------------
// Property: AB
// Propvalue: list of stone
// Propertytype: setup
// Function: Add black stones to the board. This can be used to set up
//     positions or problems. Adding is done by 'overwriting' the
//     given point with black stones. It doesn't matter what
//     was there before. Adding a stone doesn't make any prisoners
//     nor any other captures (e.g. suicide). Thus it's possible
//     to create illegal board positions.
//     Points used in stone type must be unique.
// Related: AW, AE, PL
// 
//----------------------------------------------------------------------------
// Property: AE
// Propvalue: list of point
// Propertytype: setup
// Function: Clear the given points on the board. This can be used
//     to set up positions or problems. Clearing is done by
//     'overwriting' the given points, so that they contain no
//     stones. It doesn't matter what was there before.
//     Clearing doesn't count as taking prisoners.
//     Points must be unique.
// Related: AB, AW, PL
// 
//----------------------------------------------------------------------------
// Property: AW
// Propvalue: list of stone
// Propertytype: setup
// Function: Add white stones to the board. This can be used to set up
//     positions or problems. Adding is done by 'overwriting' the
//     given points with white stones. It doesn't matter what
//     was there before. Adding a stone doesn't make any prisoners
//     nor any other captures (e.g. suicide). Thus it's possible
//     to create illegal board positions.
//     Points used in stone type must be unique.
// Related: AB, AE, PL
// 
//----------------------------------------------------------------------------
// Property: PL
// Propvalue: color
// Propertytype: setup
// Function: PL tells whose turn it is to play. This can be used when
//     setting up positions or problems.
// Related: AE, AB, AW
// Node annotation properties
// 
//----------------------------------------------------------------------------
// Property: C
// Propvalue: text
// Propertytype: -
// Function: Provides a comment text for the given node. The purpose of
//     providing both a node name and a comment is to have a short
//     identifier like "doesn't work" or "Dia. 15" that can be
//     displayed directly with the properties of the node, even if
//     the comment is turned off or shown in a separate window.
//     See text-type for more info.
// Related: N, ST, V, UC, DM, HO
// 
//----------------------------------------------------------------------------
// Property: DM
// Propvalue: double
// Propertytype: -
// Function: The position is even. SGF viewers should display a
//     message. This property may indicate main variations in
//     opening libraries (joseki) too. Thus DM[2] indicates an
//     even result for both players and that this is a main
//     variation of this joseki/opening.
//     This property must not be mixed with UC, GB or GW
//     within a node.
// Related: UC, GW, GB
// 
//----------------------------------------------------------------------------
// Property: GB
// Propvalue: double
// Propertytype: -
// Function: Something good for black. SGF viewers should display a
//     message. The property is not related to any specific place
//     on the board, but marks the whole node instead.
//     GB must not be mixed with GW, DM or UC within a node.
// Related: GW, C, UC, DM
// 
//----------------------------------------------------------------------------
// Property: GW
// Propvalue: double
// Propertytype: -
// Function: Something good for white. SGF viewers should display a
//     message. The property is not related to any specific place
//     on the board, but marks the whole node instead.
//     GW must not be mixed with GB, DM or UC within a node.
// Related: GB, C, UC, DM
// 
//----------------------------------------------------------------------------
// Property: HO
// Propvalue: double
// Propertytype: -
// Function: Node is a 'hotspot', i.e. something interesting (e.g.
//     node contains a game-deciding move).
//     SGF viewers should display a message.
//     The property is not related to any specific place
//     on the board, but marks the whole node instead.
//     Sophisticated applications could implement the navigation
//     command next/previous hotspot.
// Related: GB, GW, C, UC, DM
// 
//----------------------------------------------------------------------------
// Property: N
// Propvalue: simpletext
// Propertytype: -
// Function: Provides a name for the node. For more info have a look at
//     the C-property.
// Related: C, ST, V
// 
//----------------------------------------------------------------------------
// Property: UC
// Propvalue: double
// Propertytype: -
// Function: The position is unclear. SGF viewers should display a
//     message. This property must not be mixed with DM, GB or GW
//     within a node.
// Related: DM, GW, GB
// 
//----------------------------------------------------------------------------
// Property: V
// Propvalue: real
// Propertytype: -
// Function: Define a value for the node.  Positive values are good for
//     black, negative values are good for white.
//     The interpretation of particular values is game-specific.
//     In Go, this is the estimated score.
// Related: C, N, RE
//
//============================================================================
// Move annotation properties
// 
// Restrictions
// 
// Move annotation properties without a move (B[] or W[]) within the
// same node are senseless and therefore illegal. Applications should
// delete such properties and issue a warning.
//
// BM, TE, DO and IT are mutual exclusive, i.e. they must not be mixed
// within a single node.
//
//----------------------------------------------------------------------------
// Property: BM
// Propvalue: double
// Propertytype: move
// Function: The played move is bad.
//     Viewers should display a message.
// Related: TE, DO, IT
// 
//----------------------------------------------------------------------------
// Property: DO
// Propvalue: none
// Propertytype: move
// Function: The played move is doubtful.
//     Viewers should display a message.
// Related: BM, TE, IT
// 
//----------------------------------------------------------------------------
// Property: IT
// Propvalue: none
// Propertytype: move
// Function: The played move is interesting.
//     Viewers should display a message.
// Related: BM, DO, TE
// 
//----------------------------------------------------------------------------
// Property: TE
// Propvalue: double
// Propertytype: move
// Function: The played move is a tesuji (good move).
//     Viewers should display a message.
// Related: BM, DO, IT
//
//============================================================================
// Markup properties
// 
// Restrictions
// 
// CR, MA, SL, SQ and TR points must be unique, i.e. it's illegal to
// have two or more of these markups on the same point within a node.
//
//----------------------------------------------------------------------------
// Property: AR
// Propvalue: list of composed point ':' point
// Propertytype: -
// Function: Viewers should draw an arrow pointing FROM the first point
//     TO the second point.
//     It's illegal to specify the same arrow twice,
//     e.g. (Go) AR[aa:bb][aa:bb]. Different arrows may have the same
//     starting or ending point though.
//     It's illegal to specify a one point arrow, e.g. AR[cc:cc]
//     as it's impossible to tell into which direction the
//     arrow points.
// Related: TR, CR, LB, SL, MA, SQ, LN
// 
//----------------------------------------------------------------------------
// Property: CR
// Propvalue: list of point
// Propertytype: -
// Function: Marks the given points with a circle.
//     Points must be unique.
// Related: TR, MA, LB, SL, AR, SQ, LN
// 
//----------------------------------------------------------------------------
// Property: DD
// Propvalue: elist of point
// Propertytype: inherit
// Function: Dim (grey out) the given points.
//     Have a look at the picture to get an idea.
//     DD[] clears any setting, i.e. it undims everything.
// Related: VW
// 
//----------------------------------------------------------------------------
// Property: LB
// Propvalue: list of composed point ':' simpletext
// Propertytype: -
// Function: Writes the given text on the board. The text should be
//     centered around the given point. Note: there's no longer
//     a restriction to the length of the text to be displayed.
//     Have a look at the FF4 example file on possibilities
//     to display long labels (pictures five and six).
//     Points must be unique.
// Related: TR, CR, MA, SL, AR, SQ, LN
// 
//----------------------------------------------------------------------------
// Property: LN
// Propvalue: list of composed point ':' point
// Propertytype: -
// Function: Applications should draw a simple line form one point
//     to the other.
//     It's illegal to specify the same line twice,
//     e.g. (Go) LN[aa:bb][aa:bb]. Different lines may have the same
//     starting or ending point though.
//     It's illegal to specify a one point line, e.g. LN[cc:cc].
// Related: TR, CR, MA, SL, AR, SQ, LB
// 
// 
//----------------------------------------------------------------------------
// Property: MA
// Propvalue: list of point
// Propertytype: -
// Function: Marks the given points with an 'X'.
//     Points must be unique.
// Related: TR, CR, LB, SL, AR, SQ, LN
// 
//----------------------------------------------------------------------------
// Property: SL
// Propvalue: list of point
// Propertytype: -
// Function: Selected points. Type of markup unknown
//     (though SGB inverts the colors of the given points).
//     Points must be unique.
// Related: TR, CR, LB, MA, AR, LN
// 
//----------------------------------------------------------------------------
// Property: SQ
// Propvalue: list of point
// Propertytype: -
// Function: Marks the given points with a square.
//     Points must be unique.
// Related: TR, CR, LB, SL, AR, MA, LN
// 
//----------------------------------------------------------------------------
// Property: TR
// Propvalue: list of point
// Propertytype: -
// Function: Marks the given points with a triangle.
//     Points must be unique.
// Related: MA, CR, LB, SL, AR, LN
//
//============================================================================
// Root properties
// 
//----------------------------------------------------------------------------
// Property: AP
// Propvalue: composed simpletext ":" simpletext
// Propertytype: root
// Function: Provides the name and version number of the application used
//     to create this gametree.
//     The name should be unique and must not be changed for
//     different versions of the same program.
//     The version number itself may be of any kind, but the format
//     used must ensure that by using an ordinary string-compare,
//     one is able to tell if the version is lower or higher
//     than another version number.
//     Here's the list of known applications and their names:
// 
//     Application		     System	  Name
//     ---------------------------  -----------  --------------------
//     [CGoban:1.6.2]		     Unix	  CGoban
//     [Hibiscus:2.1]		     Windows 95   Hibiscus Go Editor
//     [IGS:5.0]				  Internet Go Server
//     [Many Faces of Go:10.0]      Windows 95   The Many Faces of Go
//     [MGT:?]			     DOS/Unix	  MGT
//     [NNGS:?]		     Unix	  No Name Go Server
//     [Primiview:3.0]   	     Amiga OS3.0  Primiview
//     [SGB:?]			     Macintosh	  Smart Game Board
//     [SmartGo:1.0]		     Windows	  SmartGo
// 
// Related: FF, GM, SZ, ST, CA
// 
//----------------------------------------------------------------------------
// Property: CA
// Propvalue: simpletext
// Propertytype: root
// Function: Provides the used charset for SimpleText and Text type.
//     Default value is 'ISO-8859-1' aka 'Latin1'.
//     Only charset names (or their aliases) as specified in RFC 1345
//     (or updates thereof) are allowed.
//     Basically this field uses the same names as MIME messages in
//     their 'charset=' field (in Content-Type).
//     RFC's can be obtained via FTP from DS.INTERNIC.NET,
//     NIS.NSF.NET, WUARCHIVE.WUSTL.EDU, SRC.DOC.IC.AC.UK
//     or FTP.IMAG.FR.
// Related: FF, C, text type
// 
//----------------------------------------------------------------------------
// Property: FF
// Propvalue: number (range: 1-4)
// Propertytype: root
// Function: Defines the used file format. For difference between those
//     formats have a look at the history of SGF.
//     Default value: 1
//     Applications must be able to deal with different file formats
//     within a collection.
// Related: GM, SZ, ST, AP, CA
// 
//----------------------------------------------------------------------------
// Property: GM
// Propvalue: number (range: 1-16)
// Propertytype: root
// Function: Defines the type of game, which is stored in the current
//     gametree. The property should help applications
//     to reject games, they cannot handle.
//     Valid numbers are: Go = 1, Othello = 2, chess = 3,
//     Gomoku+Renju = 4, Nine Men's Morris = 5, Backgammon = 6,
//     Chinese chess = 7, Shogi = 8, Lines of Action = 9,
//     Ataxx = 10, Hex = 11, Jungle = 12, Neutron = 13,
//     Philosopher's Football = 14, Quadrature = 15, Trax = 16,
//     Tantrix = 17, Amazons = 18, Octi = 19, Gess = 20,
//     Twixt = 21, Zertz = 22, Plateau = 23, Yinsh = 24,
//     Punct = 25, Gobblet = 26, hive = 27, Exxit = 28,
//     Hnefatal = 29, Kuba = 30, Tripples = 31, Chase = 32,
//     Tumbling Down = 33, Sahara = 34, Byte = 35, Focus = 36,
//     Dvonn = 37, Tamsk = 38, Gipf = 39, Kropki = 40.
//     Default value: 1
//     Different kind of games may appear within a collection.
// Related: FF, SZ, ST, AP, CA
// 
//----------------------------------------------------------------------------
// Property: ST
// Propvalue: number (range: 0-3)
// Propertytype: root
// Function: Defines how variations should be shown (this is needed to
//     synchronize the comments with the variations). If ST is omitted
//     viewers should offer the possibility to change the mode online.
//     Basically most programs show variations in two ways:
//     as markup on the board (if the variation contains a move)
//     and/or as a list (in a separate window).
//     The style number consists two options.
//     1) show variations of successor node (children) (value: 0)
//        show variations of current node   (siblings) (value: 1)
//        affects markup & list
//     2) do board markup         (value: 0)
//        no (auto-) board markup (value: 2)
//        affects markup only.
//        Using no board markup could be used in problem collections
//        or if variations are marked by subsequent properties.
//        Viewers should take care, that the automatic variation
//        board markup DOESN'T overwrite any markup of other
//        properties.
//     The  final number is calculated by adding the values of each
//     option.	Example: 3 = no board markup/variations of current node
//      1 = board markup/variations of current node
//     Default value: 0
// Related: C, FF, GM, SZ, AP, CA
// 
//----------------------------------------------------------------------------
// Property: SZ
// Propvalue: (number | composed number ':' number)
// Propertytype: root
// Function: Defines the size of the board. If only a single value
//     is given, the board is a square; with two numbers given,
//     rectangular boards are possible.
//     If a rectangular board is specified, the first number specifies
//     the number of columns, the second provides the number of rows.
//     Square boards must not be defined using the compose type
//     value: e.g. SZ[19:19] is illegal.
//     The valid range for SZ is any size greater or equal to 1x1.
//     For Go games the maximum size is limited to 52x52.
//     Default value: game specific
//            for Go: 19 (square board)
//            for Chess: 8 (square board)
//     Different board sizes may appear within a collection.
//     See move-/point-type for more info.
// Related: FF, GM, ST, AP, CA
//
//============================================================================
// Game info properties
// 
//----------------------------------------------------------------------------
// Property: AN
// Propvalue: simpletext
// Propertytype: game-info
// Function: Provides the name of the person, who made the annotations
//     to the game.
// Related: US, SO, CP
// 
//----------------------------------------------------------------------------
// Property: BR
// Propvalue: simpletext
// Propertytype: game-info
// Function: Provides the rank of the black player.
//     For Go (GM[1]) the following format is recommended:
//     "..k" or "..kyu" for kyu ranks and
//     "..d" or "..dan" for dan ranks.
//     Go servers may want to add '?' for an uncertain rating and
//     '*' for an established rating.
// Related: PB, BT, WR
// 
//----------------------------------------------------------------------------
// Property: BT
// Propvalue: simpletext
// Propertytype: game-info
// Function: Provides the name of the black team, if game was part of a
//     team-match (e.g. China-Japan Supermatch).
// Related: PB, PW, WT
// 
//----------------------------------------------------------------------------
// Property: CP
// Propvalue: simpletext
// Propertytype: game-info
// Function: Any copyright information (e.g. for the annotations) should
//     be included here.
// Related: US, SO, AN
// 
//----------------------------------------------------------------------------
// Property: DT
// Propvalue: simpletext
// Propertytype: game-info
// Function: Provides the date when the game was played.
//     It is MANDATORY to use the ISO-standard format for DT.
//     Note: ISO format implies usage of the Gregorian calendar.
//     Syntax:
//     "YYYY-MM-DD" year (4 digits), month (2 digits), day (2 digits)
//     Do not use other separators such as "/", " ", "," or ".".
//     Partial dates are allowed:
//     "YYYY" - game was played in YYYY
//     "YYYY-MM" - game was played in YYYY, month MM
//     For games that last more than one day: separate other dates
//     by a comma (no spaces!); following shortcuts may be used:
//     "MM-DD" - if preceded by YYYY-MM-DD, YYYY-MM, MM-DD, MM or DD
//     "MM" - if preceded by YYYY-MM or MM
//     "DD" - if preceded by YYYY-MM-DD, MM-DD or DD
//     Shortcuts acquire the last preceding YYYY and MM (if
//     necessary).
//     Note: interpretation is done from left to right.
//     Examples:
//     1996-05,06 = played in May,June 1996
//     1996-05-06,07,08 = played on 6th,7th,8th May 1996
//     1996,1997 = played in 1996 and 1997
//     1996-12-27,28,1997-01-03,04 = played on 27th,28th
//     of December 1996 and on 3rd,4th January 1997
//     Note: it's recommended to use shortcuts whenever possible,
//     e.g. 1997-05-05,06 instead of 1997-05-05,1997-05-06
// Related: EV, RO, PC, RU, RE, TM
// 
//----------------------------------------------------------------------------
// Property: EV
// Propvalue: simpletext
// Propertytype: game-info
// Function: Provides the name of the event (e.g. tournament).
//     Additional information (e.g. final, playoff, ..)
//     shouldn't be included (see RO).
// Related: GC, RO, DT, PC, RU, RE, TM
// 
//----------------------------------------------------------------------------
// Property: GN
// Propvalue: simpletext
// Propertytype: game-info
// Function: Provides a name for the game. The name is used to
//     easily find games within a collection.
//     The name should therefore contain some helpful information
//     for identifying the game. 'GameName' could also be used
//     as the file-name, if a collection is split into
//     single files.
// Related: GC, EV, DT, PC, RO, ID
// 
//----------------------------------------------------------------------------
// Property: GC
// Propvalue: text
// Propertytype: game-info
// Function: Provides some extra information about the following game.
//     The intend of GC is to provide some background information
//     and/or to summarize the game itself.
// Related: GN, ON, AN, CP
// 
//----------------------------------------------------------------------------
// Property: ON
// Propvalue: simpletext
// Propertytype: game-info
// Function: Provides some information about the opening played
//     (e.g. san-ren-sei, Chinese fuseki, etc.).
// Related: GN, GC
// 
//----------------------------------------------------------------------------
// Property: OT
// Propvalue: simpletext
// Propertytype: game-info
// Function: Describes the method used for overtime (byo-yomi).
//     Examples: "5 mins Japanese style, 1 move / min",
//       "25 moves / 10 min".
// Related: TM, BL, WL, OB, OW
// 
//----------------------------------------------------------------------------
// Property: PB
// Propvalue: simpletext
// Propertytype: game-info
// Function: Provides the name of the black player.
// Related: PW, BT, WT
// 
//----------------------------------------------------------------------------
// Property: PC
// Propvalue: simpletext
// Propertytype: game-info
// Function: Provides the place where the games was played.
// Related: EV, DT, RO, RU, RE, TM
// 
//----------------------------------------------------------------------------
// Property: PW
// Propvalue: simpletext
// Propertytype: game-info
// Function: Provides the name of the white player.
// Related: PB, BT, WT
// 
//----------------------------------------------------------------------------
// Property: RE
// Propvalue: simpletext
// Propertytype: game-info
// Function: Provides the result of the game. It is MANDATORY to use the
//     following format:
//     "0" (zero) or "Draw" for a draw (jigo),
//     "B+" ["score"] for a black win and
//     "W+" ["score"] for a white win
//     Score is optional (some games don't have a score e.g. chess).
//     If the score is given it has to be given as a real value,
//     e.g. "B+0.5", "W+64", "B+12.5"
//     Use "B+R" or "B+Resign" and "W+R" or "W+Resign" for a win by
//     resignation. Applications must not write "Black resigns".
//     Use "B+T" or "B+Time" and "W+T" or "W+Time" for a win on time,
//     "B+F" or "B+Forfeit" and "W+F" or "W+Forfeit" for a win by
//     forfeit,
//     "Void" for no result or suspended play and
//     "?" for an unknown result.
// 
// Related: EV, DT, PC, RO, RU, TM
// 
//----------------------------------------------------------------------------
// Property: RO
// Propvalue: simpletext
// Propertytype: game-info
// Function: Provides round-number and type of round. It should be
//     written in the following way: RO[xx (tt)], where xx is the
//     number of the round and (tt) the type:
//     final, playoff, league, ...
// Related: EV, DT, PC, RU, RE, TM
// 
//----------------------------------------------------------------------------
// Property: RU
// Propvalue: simpletext
// Propertytype: game-info
// Function: Provides the used rules for this game.
//     Because there are many different rules, SGF requires
//     mandatory names only for a small set of well known rule sets.
//     Note: it's beyond the scope of this specification to give an
//     exact specification of these rule sets.
//     Mandatory names for Go (GM[1]):
//     "AGA" (rules of the American Go Association)
//     "GOE" (the Ing rules of Goe)
//     "Japanese" (the Nihon-Kiin rule set)
//     "NZ" (New Zealand rules)
// 
// Related: EV, DT, PC, RO, RE, TM
// 
//----------------------------------------------------------------------------
// Property: SO
// Propvalue: simpletext
// Propertytype: game-info
// Function: Provides the name of the source (e.g. book, journal, ...).
// Related: US, AN, CP
// 
//----------------------------------------------------------------------------
// Property: TM
// Propvalue: real
// Propertytype: game-info
// Function: Provides the time limits of the game.
//     The time limit is given in seconds.
// Related: EV, DT, PC, RO, RU, RE
// 
//----------------------------------------------------------------------------
// Property: US
// Propvalue: simpletext
// Propertytype: game-info
// Function: Provides the name of the user (or program), who entered
//     the game.
// Related: SO, AN, CP
// 
//----------------------------------------------------------------------------
// Property: WR
// Propvalue: simpletext
// Propertytype: game-info
// Function: Provides the rank of the white player. For recommended
//     format see BR.
// Related: PW, WT, BR
// 
//----------------------------------------------------------------------------
// Property: WT
// Propvalue: simpletext
// Propertytype: game-info
// Function: Provide the name of the white team, if game was part of a
//     team-match (e.g. China-Japan Supermatch).
// Related: PB, PW, BT
//
//============================================================================
// Timing properties
// 
//----------------------------------------------------------------------------
// Property: BL
// Propvalue: real
// Propertytype: move
// Function: Time left for black, after the move was made.
//     Value is given in seconds.
// Related: TM, OT, WL, OB, OW
// 
//----------------------------------------------------------------------------
// Property: OB
// Propvalue: number
// Propertytype: move
// Function: Number of black moves left (after the move of this node was
//     played) to play in this byo-yomi period.
// Related: TM, OT, BL, WL, OW
// 
//----------------------------------------------------------------------------
// Property: OW
// Propvalue: number
// Propertytype: move
// Function: Number of white moves left (after the move of this node was
//     played) to play in this byo-yomi period.
// Related: TM, OT, BL, WL, OB
// 
//----------------------------------------------------------------------------
// Property: WL
// Propvalue: real
// Propertytype: move
// Function: Time left for white after the move was made.
//     Value is given in seconds.
// Related: TM, OT, BL, OB, OW
//
//============================================================================
// Miscellaneous properties
// 
//----------------------------------------------------------------------------
// Property: FG
// Propvalue: none | composition of number ":" SimpleText
// Propertytype: -
// Function: The figure property is used to divide a game into
//     different figures for printing: a new figure starts at the
//     node containing a figure property.
//     If the value is not empty then
//     - Simpletext provides a name for the diagram
//     - Number specifies some flags (for printing).
//       These flags are:
//     - coordinates on/off (value: 0/1)
//     - diagram name on/off (value: 0/2)
//     - list moves not shown in figure on/off (value: 0/4)
//       Some moves can't be shown in a diagram (e.g. ko
//       captures in Go) - these moves may be listed as text.
//     - remove captured stones on/off (value: 0/256)
//       'remove off' means: keep captured stones in the
//       diagram and don't overwrite stones played earlier -
//       this is the way diagrams are printed in books.
//       'remove on' means: capture and remove the stones from
//       the display - this is the usual viewer mode.
//       This flag is specific to Go (GM[1]).
//     - hoshi dots on/off (value: 0/512)
//       This flag is specific to Go (GM[1]).
//     - Ignore flags on/off (value: 32768)
//       If on, then all other flags should be ignored and
//       the application should use its own defaults.
//       The final number is calculated by summing up all flag values.
//       E.g. 515 = coordinates and diagram name off, remove captured
//       stones, list unshown moves, hoshi dots off;
//       257 = coordinates off, diagram name on, list unshown moves,
//       don't remove captured stones, hoshi dots on.
//       (this is how diagrams are printed in e.g. Go World)
//     Note: FG combined with VW, MN and PM are mighty tools to print
//     and compile diagrams.
// Related: MN, PM, VW
// 
//----------------------------------------------------------------------------
// Property: PM
// Propvalue: number
// Propertytype: inherit
// Function: This property is used for printing.
//     It specifies how move numbers should be printed.
//     0 ... don't print move numbers
//     1 ... print move numbers as they are
//     2 ... print 'modulo 100' move numbers
//     This mode is usually used in books or magazines.
//     Note: Only the first move number is calculated
//     'modulo 100' and the obtained number is increased
//     for each move in the diagram.
//     E.g. A figure containing moves
//      32-78  is printed as moves 32-78
//     102-177 is printed as moves  2-77
//      67-117 is printed as moves 67-117
//     154-213 is printed as moves 54-113
//     Default value: 1
// Related: MN, FG
// 
//----------------------------------------------------------------------------
// Property: VW
// Propvalue: elist of point
// Propertytype: inherit
// Function: View only part of the board. The points listed are
//     visible, all other points are invisible.
//     Note: usually the point list is given in compressed
//     format (see 'point' type)!
//     Points have to be unique.
//     Have a look at the picture to get an idea.
//     VW[] clears any setting, i.e. the whole board is
//     visible again.
// Related: DD, PM, FG
//
