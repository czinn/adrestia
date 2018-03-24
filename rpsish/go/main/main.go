package main

import (
	"fmt"
	"github.com/czinn/adrestia/rpsish"
)

func main() {
	game := rpsish.NewRpsishGame(5, 5, [][]int{
		[]int{0, 3, 1, 4, -4},
		[]int{-3, 0, 2, 2, -1},
		[]int{-1, -2, 0, -2, 4},
		[]int{-4, -2, 2, 0, 1},
		[]int{4, 1, -4, -1, 0},
	})
	strategies := []rpsish.Strategy{rpsish.NewRandomStrategy(), rpsish.NewRandomStrategy()}
	for !game.Finished() {
		fmt.Printf("Game: %s\n", game)
		player := game.CurrentPlayer()
		view := game.View(player)
		fmt.Printf("View(%d): %s\n", player, view)
		action := strategies[player].ChooseAction(game.View(player))
		fmt.Printf("Action: %d\n", action)
		game.PerformAction(action)
		fmt.Println()
	}
	fmt.Printf("Game: %s\n", game)
	fmt.Printf("%f : %f\n", game.Payoff(0), game.Payoff(1))
}
