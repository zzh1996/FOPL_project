Begin
	Var x End
	Function test Paras i Begin
		If Gt i 10 Begin
			Return Plus i Minus i 1 
			End
		Else Begin 
			If Gt i 5 Begin
				Return Mult i Div i 2
				End
			Else Begin
				Return Mod i 2
				End
		End	
	End
	Read x
	While Gt x 0 Begin
		Print Apply test Argus x End
		Assign x Minus x 1
	End
End