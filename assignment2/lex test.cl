#name "test.cl"
#4 ERROR "Unmatched *)"
#6 CLASS
#6 TYPEID CellularAutomaton
#6 INHERITS
#6 TYPEID IO
#6 '{'
#7 OBJECTID population_map
#7 ':'
#7 TYPEID String
#7 ';'
#9 OBJECTID init
#9 '('
#9 OBJECTID map
#9 ':'
#9 TYPEID String
#9 ')'
#9 ':'
#9 TYPEID SELF_TYPE
#9 '{'
#10 '{'
#11 OBJECTID population_map
#11 ASSIGN
#11 OBJECTID map
#11 ';'
#12 OBJECTID self
#12 ';'
#13 '}'
#14 '}'
#14 ';'
#16 OBJECTID print
#16 '('
#16 ')'
#16 ':'
#16 TYPEID SELF_TYPE
#16 '{'
#17 '{'
#18 OBJECTID out_string
#18 '('
#18 OBJECTID population_map
#18 '.'
#18 OBJECTID concat
#18 '('
#18 STR_CONST "\n"
#18 ')'
#18 ')'
#18 ';'
#19 OBJECTID self
#19 ';'
#20 '}'
#21 '}'
#21 ';'
#23 OBJECTID num_cells
#23 '('
#23 ')'
#23 ':'
#23 TYPEID Int
#23 '{'
#24 OBJECTID population_map
#24 '.'
#24 OBJECTID length
#24 '('
#24 ')'
#25 '}'
#25 ';'
#27 OBJECTID cell
#27 '('
#27 OBJECTID position
#27 ':'
#27 TYPEID Int
#27 ')'
#27 ':'
#27 TYPEID String
#27 '{'
#28 OBJECTID population_map
#28 '.'
#28 OBJECTID substr
#28 '('
#28 OBJECTID position
#28 ','
#28 INT_CONST 1
#28 ')'
#29 '}'
#29 ';'
#31 OBJECTID cell_left_neighbor
#31 '('
#31 OBJECTID position
#31 ':'
#31 TYPEID Int
#31 ')'
#31 ':'
#31 TYPEID String
#31 '{'
#32 IF
#32 OBJECTID position
#32 '='
#32 INT_CONST 0
#32 THEN
#33 OBJECTID cell
#33 '('
#33 OBJECTID num_cells
#33 '('
#33 ')'
#33 '-'
#33 INT_CONST 1
#33 '-'
#33 ERROR ">"
#33 ')'
#34 ELSE
#35 OBJECTID cell
#35 '('
#35 OBJECTID position
#35 '-'
#35 INT_CONST 1
#35 ')'
#36 FI
#37 '}'
#37 ';'
#39 OBJECTID cell_right_neighbor
#39 '('
#39 OBJECTID position
#39 ':'
#39 TYPEID Int
#39 ')'
#39 ':'
#39 TYPEID String
#39 '{'
#40 IF
#40 OBJECTID position
#40 '='
#40 OBJECTID num_cells
#40 '('
#40 ')'
#40 '-'
#40 INT_CONST 1
#40 THEN
#41 OBJECTID cell
#41 '('
#41 INT_CONST 0
#41 ')'
#42 ELSE
#43 OBJECTID cell
#43 '('
#43 OBJECTID position
#43 '+'
#43 INT_CONST 1
#43 ')'
#44 FI
#45 '}'
#45 ';'
#49 OBJECTID cell_at_next_evolution
#49 '('
#49 OBJECTID position
#49 ':'
#49 TYPEID Int
#49 ')'
#49 ':'
#49 TYPEID String
#49 '{'
#50 IF
#50 '('
#50 IF
#50 OBJECTID cell
#50 '('
#50 OBJECTID position
#50 ')'
#50 '='
#50 STR_CONST "X"
#50 THEN
#50 INT_CONST 1
#50 ELSE
#50 INT_CONST 0
#50 FI
#51 '+'
#51 IF
#51 OBJECTID cell_left_neighbor
#51 '('
#51 OBJECTID position
#51 ')'
#51 '='
#51 STR_CONST "X"
#51 THEN
#51 INT_CONST 1
#51 ELSE
#51 INT_CONST 0
#51 FI
#52 '+'
#52 IF
#52 OBJECTID cell_right_neighbor
#52 '('
#52 OBJECTID position
#52 ')'
#52 '='
#52 STR_CONST "X"
#52 THEN
#52 INT_CONST 1
#52 ELSE
#52 INT_CONST 0
#52 FI
#53 '='
#53 INT_CONST 1
#53 ')'
#54 THEN
#55 STR_CONST ""
#56 ELSE
#57 ERROR "'"
#57 '.'
#57 ERROR "'"
#58 FI
#59 '}'
#59 ';'
#61 OBJECTID evolve
#61 '('
#61 ')'
#61 ':'
#61 TYPEID SELF_TYPE
#61 '{'
#62 '('
#62 LET
#62 OBJECTID position
#62 ':'
#62 TYPEID Int
#62 IN
#63 '('
#63 LET
#63 OBJECTID num
#63 ':'
#63 TYPEID Int
#63 ASSIGN
#63 OBJECTID num_cells
#63 ERROR "["
#63 ERROR "]"
#63 IN
#64 '('
#64 LET
#64 OBJECTID temp
#64 ':'
#64 TYPEID String
#64 IN
#65 '{'
#66 WHILE
#66 OBJECTID position
#66 '<'
#66 OBJECTID num
#66 LOOP
#67 '{'
#68 OBJECTID temp
#68 ASSIGN
#68 OBJECTID temp
#68 '.'
#68 OBJECTID concat
#68 '('
#68 OBJECTID cell_at_next_evolution
#68 '('
#68 OBJECTID position
#68 ')'
#68 ')'
#68 ';'
#69 OBJECTID position
#69 ASSIGN
#69 OBJECTID position
#69 '+'
#69 INT_CONST 1
#69 ';'
#70 '}'
#71 POOL
#71 ';'
#72 OBJECTID population_map
#72 ASSIGN
#72 OBJECTID temp
#72 ';'
#73 OBJECTID self
#73 ';'
#74 '}'
#75 ')'
#75 ')'
#75 ')'
#76 '}'
#76 ';'
#77 '}'
#77 ';'
#79 CLASS
#79 TYPEID Main
#79 '{'
#80 OBJECTID cells
#80 ':'
#80 TYPEID CellularAutomaton
#80 ';'
#81 OBJECTID s
#81 ':'
#81 TYPEID Stiring
#81 ';'
#82 OBJECTID main
#82 '('
#82 ')'
#82 ':'
#82 TYPEID SELF_TYPE
#82 '{'
#83 '{'
#84 OBJECTID cells
#84 ASSIGN
#84 '('
#84 NEW
#84 TYPEID CellularAutomaton
#84 ')'
#84 '.'
#84 OBJECTID init
#84 '('
#84 STR_CONST "         X         "
#84 ')'
#84 ';'
#85 OBJECTID s
#85 ASSIGN
#85 STR_CONST "sjfklasdfjkajfklajfdkajdfkjfdkafjdklafj\t\nkadjfkasjfdlakjfdhgajfiejoifjfigjigjgoijgihgiehiejgejgihgiejgsijdgkhdghdgkdjkfjakfjakghakdhgkjg;adjghkagh;ahdg;dgjdhgag;jdgkjgkhdkgajdkjgdkghdkhgk;djkghak;dkdghdhgkd;jgkdjkhgkadj;gkhgkagj;agjdkhg;dgka;gieihenvaiijighadkgjdkjaskdgj;akdjskadjf;lfjdks;jfdkjfslkdfjsl;fjlsdfjksfdl;js;dlfkjskdjfsdkj;afdgkg;jsdgkjg;gka:"
#86 OBJECTID s
#86 ASSIGN
#87 ERROR "Unterminated string constant"
#87 OBJECTID kdfjsalkdjf
#87 ';'
#87 OBJECTID a
#88 ERROR "Unterminated string constant"
#88 OBJECTID cells
#88 '.'
#88 OBJECTID print
#88 '('
#88 ')'
#88 ';'
#89 '('
#89 LET
#89 OBJECTID countdown
#89 ':'
#89 TYPEID Int
#89 ASSIGN
#89 INT_CONST 20
#89 IN
#90 WHILE
#90 OBJECTID countdown
#90 ERROR ">"
#90 INT_CONST 0
#90 LOOP
#91 '{'
#92 OBJECTID cells
#92 '.'
#92 OBJECTID evolve
#92 '('
#92 ')'
#92 ';'
#93 OBJECTID cells
#93 '.'
#93 OBJECTID print
#93 '('
#93 ')'
#93 ';'
#94 OBJECTID countdown
#94 ASSIGN
#94 OBJECTID countdown
#94 '-'
#94 INT_CONST 1
#94 ';'
#96 POOL
#97 ERROR "Unterminated string constant"
#97 ')'
#97 ';'
#102 ERROR "EOF in comment"