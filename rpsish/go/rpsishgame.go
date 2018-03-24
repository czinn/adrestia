package rpsish

import (
	"bytes"
	"crypto/md5"
	"encoding/binary"
	"strconv"
)

// There are 4K-2 turns because each player picks 2K-1 symbols during the K rounds.

type RpsishGameView struct {
	symbols      int     // number of symbols (N)
	rounds       int     // number of rounds (K)
	payoff       [][]int // payoff table (A)
	turn         int     // current turn (0 to 4K-3)
	myactions    []int   // actions by viewing player
	theiractions []int   // public actions by other player
}

func (s RpsishGameView) Symbols() int {
	return s.symbols
}

func (s RpsishGameView) Rounds() int {
	return s.rounds
}

func (s RpsishGameView) Payoff() [][]int {
	return s.payoff
}

func (s RpsishGameView) Turn() int {
	return s.turn
}

func (s RpsishGameView) MyActions() []int {
	return s.myactions
}

func (s RpsishGameView) TheirActions() []int {
	return s.theiractions
}

func (s RpsishGameView) Actions() []Action {
	var actions = make([]Action, s.symbols)
	for i := 0; i < s.symbols; i++ {
		actions[i] = Action(i)
	}
	return actions
}

func (s RpsishGameView) HashKey() int {
	h := md5.New()
	binary.Write(h, binary.LittleEndian, s.symbols)
	binary.Write(h, binary.LittleEndian, s.rounds)
	binary.Write(h, binary.LittleEndian, s.payoff)
	binary.Write(h, binary.LittleEndian, s.turn)
	for _, action := range s.myactions {
		binary.Write(h, binary.LittleEndian, action)
	}
	for _, action := range s.theiractions {
		binary.Write(h, binary.LittleEndian, action)
	}
	result, _ := binary.Varint(h.Sum(nil))
	return int(result)
}

func listToString(list []int) string {
	var buffer bytes.Buffer
	buffer.WriteByte('(')
	for j, elem := range list {
		if j > 0 {
			buffer.WriteByte(' ')
		}
		buffer.WriteString(strconv.Itoa(elem))
	}
	buffer.WriteByte(')')
	return buffer.String()
}

func (s RpsishGameView) String() string {
	var buffer bytes.Buffer
	buffer.WriteByte('(')
	buffer.WriteString(listToString(s.myactions))
	buffer.WriteByte(' ')
	buffer.WriteString(listToString(s.theiractions))
	buffer.WriteByte(')')
	return buffer.String()
}

type RpsishGame struct {
	symbols int      // number of symbols (N)
	rounds  int      // number of rounds (K)
	payoff  [][]int  // payoff table (A)
	turn    int      // current turn (0 to 4K-3)
	actions [2][]int // actions played by both players
}

func NewRpsishGame(symbols int, rounds int, payoff [][]int) RpsishGame {
	return RpsishGame{
		symbols,
		rounds,
		payoff,
		0,
		[2][]int{make([]int, rounds*2-1)[:0], make([]int, rounds*2-1)[:0]},
	}
}

func (s RpsishGame) Finished() bool {
	return s.turn == 4*s.rounds-2
}

func (s RpsishGame) CurrentPlayer() int {
	return s.turn % 2
}

func (s RpsishGame) Payoff(player int) float64 {
	symbolCounts := [2][]int{make([]int, s.symbols), make([]int, s.symbols)}
	for i, actions := range s.actions {
		for _, action := range actions {
			symbolCounts[i][action]++
		}
	}
	score := 0.0
	for i, a := range symbolCounts[player] {
		for j, b := range symbolCounts[1-player] {
			score += float64(a * b * s.payoff[i][j])
		}
	}
	return score
}

func min(x, y int) int {
	if x <= y {
		return x
	}
	return y
}

func (s RpsishGame) View(player int) RpsishGameView {
	numVisibleActions := min(len(s.actions[player]), len(s.actions[1-player])) / 2
	visibleActions := make([]int, numVisibleActions)
	for i, _ := range visibleActions {
		visibleActions[i] = s.actions[1-player][i*2+1]
	}
	return RpsishGameView{
		s.symbols,
		s.rounds,
		s.payoff,
		s.turn,
		s.actions[player],
		visibleActions,
	}
}

func (s *RpsishGame) PerformAction(action Action) {
	player := s.CurrentPlayer()
	s.actions[player] = append(s.actions[player], int(action))
	s.turn++
}

func (s RpsishGame) Clone() RpsishGame {
	clone := RpsishGame{
		s.symbols,
		s.rounds,
		s.payoff,
		s.turn,
		[2][]int{
			make([]int, len(s.actions[0]), s.rounds*2-1),
			make([]int, len(s.actions[1]), s.rounds*2-1),
		},
	}
	for i, actions := range s.actions {
		copy(clone.actions[i], actions)
	}
	return clone
}

func (s RpsishGame) HashKey() int {
	h := md5.New()
	binary.Write(h, binary.LittleEndian, s.symbols)
	binary.Write(h, binary.LittleEndian, s.rounds)
	binary.Write(h, binary.LittleEndian, s.payoff)
	binary.Write(h, binary.LittleEndian, s.turn)
	for _, actions := range s.actions {
		for _, action := range actions {
			binary.Write(h, binary.LittleEndian, action)
		}
	}
	result, _ := binary.Varint(h.Sum(nil))
	return int(result)
}

func (s RpsishGame) String() string {
	var buffer bytes.Buffer
	buffer.WriteByte('(')
	for i, actions := range s.actions {
		if i > 0 {
			buffer.WriteByte(' ')
		}
		buffer.WriteString(listToString(actions))
	}
	buffer.WriteByte(')')
	return buffer.String()
}
