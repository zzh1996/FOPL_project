Begin
	Var x End
	Function fact Paras i
	Begin
		If Lt i 2
		Begin
			Return 1
		End
		Else
		Begin
			Return Mult i Apply fact Argus Minus i 1 End
		End
	End
	Read x
	Print Apply fact Argus x End
End
