Begin
    Function f Paras
    Begin
        Print 100
    End
    Function g Paras a b c
    Begin
        Print a
        Print b
        Print c
    End
    Function h Paras
    Begin
        Print 10
        Return Plus 10 5
        Print 20
    End
    Var x End
    Var y End
    Var z End
    Call f Argus End
    Read x
    Read y
    Read z
    Assign z Plus z 1
    Call g Argus x y z End
    Print Apply f Argus End
    Print Apply h Argus End
    Function r Paras
    Begin
        Var t End
        Assign t 1
        If Eq t 1
        Begin
            Print 100
            If Eq t 1
            Begin
                Print 101
                Return 999
                Print 102
            End Else Begin End
            Print 103
        End Else Begin End
        Print 104
    End
    Print Apply r Argus End
    Function s Paras
    Begin
        Var t End
        Assign t 5
        Function eq Paras a b
        Begin
            Print a
            If Eq a b
            Begin
                Return 1
            End Else Begin
                Return 0
            End
        End
        While Gt t 0
        Begin
            Print t
            If Eq Apply eq Argus t 2 End 1
            Begin
                Return 998
            End Else Begin End
            Print t
            Assign t Minus t 1
        End
        Print 997
    End
    Print Apply s Argus End
End