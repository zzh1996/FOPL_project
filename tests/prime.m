Begin
    Function isprime Paras n
    Begin
        Var i End
        Assign i 2
        While Lt Mult i i Plus n 1
        Begin
            If Eq Mod n i 0
            Begin
                Return 0
            End Else Begin End
            Assign i Plus i 1
        End
        Return 1
    End
    Var x End
    Var i End
    Read x
    Assign i 2
    While Lt i x
    Begin
        If Eq Apply isprime Argus i End 1
        Begin
            Print i
        End Else Begin End
        Assign i Plus i 1
    End
End
