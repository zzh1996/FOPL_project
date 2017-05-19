Begin
    Function f Paras g h Begin
        Function t Paras x Begin
            Return Apply g Argus h x End
        End
        Return t
    End
    Function add Paras x y Begin
        Return Plus x y
    End
    Var a End
    Assign a Apply f Argus add 1 End
    Print Apply a Argus 1 End
End
