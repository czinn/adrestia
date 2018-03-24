package rpsish

import (
	"math/rand"
	"time"
)

type RandomStrategy struct {
	rng *rand.Rand
}

func NewRandomStrategy() RandomStrategy {
	return RandomStrategy{
		rand.New(rand.NewSource(time.Now().UTC().UnixNano())),
	}
}

func (s RandomStrategy) ChooseAction(view GameView) Action {
	actions := view.Actions()
	return actions[s.rng.Int()%len(actions)]
}
