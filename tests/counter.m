Begin 
    Function makecounter Paras init
    Begin
        Function counter Paras inc
        Begin
            Assign init Plus init inc
            Return init
        End
        Return counter
    End

    Var ca cb End
    Assign ca Apply makecounter Argus 1 End
    Print Apply ca Argus 2 End
    Assign cb Apply makecounter Argus 10 End
    Print Apply cb Argus 10 End
    Print Apply ca Argus 5 End
    Print Apply cb Argus 80 End
End
