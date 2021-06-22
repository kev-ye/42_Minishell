- [x] echo options multi -n
- [x] seg with exit cmd and no args - (returns code 139 - it may help...)
- [x] export env name with spaces (ex: export "ksjfhg  fglkj")
- [x] change PWD and OLDPWD in `cd' cmd
- [x] unset PATH, env must not be found (ex: unset PATH; env)
- [x] .. : command not found
- [x] . -> is short for "source cmd", and source is a builtin cmd, we don't need do this
- [x] cat << f1 (then type anything, enter, and CTRL-D - bash displays the last line entered)
- [x] mix case: ls | wc > f1 | wc
- [x] mix case: ls | wc << f1
- [x] mix case: ls | wc > f1 | echo test | wc > f2
- [x] mix case: ls ; ls | wc ; echo test ; ls | wc << 1 ; cat << 1 << 2 ; ls (doube input first)
- [x] ls > f1 | echo test < f2 | echo test > f3 | echo test > f4 ; echo test > f5
- [x] ls ; wc < f1 ; ls > f1 ; echo test
- [x] wc < f1 : seg
- [x] ls | ./minishill : command not found, idk why
- [x] cat << f$?
- [x] export lol ; export "lol = fuck" tt ---> result : ttl ???
- [ ] check env, maybe is not complet
- [ ] some change with exec -> need to check (unset PATH get some new case)
- [ ] add signal for CTRL-\


cmd1    cm2    cmd3   cmd3  cmd4
     0 1    2 3    4 5    6 7 