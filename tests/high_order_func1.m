Begin 
    Var x End
    Assign x 4 
    Function g Paras z
    Begin
        Return Mult x z 
    End 
    Function f Paras h
    Begin 
        Var x End
        Assign x 7 
        Return Plus x Apply h Argus 3 End
    End
    Print Apply f Argus g End
End
