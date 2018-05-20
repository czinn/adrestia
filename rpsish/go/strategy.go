package rpsish

type Strategy interface {
	ChooseAction(view GameView) Action
}
