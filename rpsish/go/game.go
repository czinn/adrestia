package rpsish

type Action int

type GameView interface {
	Actions() []Action
	HashKey() int
	String() string
}

type Game interface {
	Finished() bool
	CurrentPlayer() int
	Payoff(player int) float64
	View(player int) GameView
	PerformAction(action Action)
	Clone() Game
	HashKey() int
	String() string
}
