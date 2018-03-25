mod rpsish;

use std::cmp::Ordering;

/* A GameState is a type for which there are two associated types: a View and an Action. The View
 * is a projection of the GameState into a subset that a particular player can see. An Action is
 * generated by the players and applied to the GameState to mutate it. */
trait GameState<'a> {
  type View;
  type Action;

  fn done(&self) -> bool;
  fn apply_action(&mut self, &Self::Action);
  fn moves(&self) -> Vec<Self::Action>;
  fn to_view(&self, usize) -> Self::View;
}

/* A Player for a particular game can make moves on the game given a GameState::View. */
trait Player<'a> {
  type Game: GameState<'a>;

  /* Note: We have to explicitly annotate Self::Game as GameState<'a> in this signature because
   * https://github.com/rust-lang/rust/issues/38078 ... and we can't define a type alias for it
   * because https://github.com/rust-lang/rust/issues/29661 -- but trust me guys, Rust is good */
  fn make_move(&self, usize, &<Self::Game as GameState<'a>>::View)
    -> <Self::Game as GameState<'a>>::Action;
}

fn main() {
  let config = rpsish::RpsishConfig {
    num_symbols: 3,
    /* 0 beats 1 beats 2 beats 0 */
    payoffs: vec![
      vec![0, 1, -1],
      vec![-1, 0, 1],
      vec![1, -1, 0],
    ],
    num_rounds: 5,
  };
  let mut state = rpsish::RpsishState {
    config: &config,
    player: 0,
    public_symbols:  [Vec::new(), Vec::new()],
    private_symbols: [Vec::new(), Vec::new()],
  };
  let players: Vec<Box<Player<Game=rpsish::RpsishState>>> = vec![
    Box::new(rpsish::SimpletonPlayer),
    Box::new(rpsish::ConstantPlayer { public_symbol: 0, private_symbol: 1 }),
  ];
  while !state.done() {
    let view = state.to_view(state.player);
    let action = players[state.player].make_move(state.player, &view);
    state.apply_action(&action);
  }

  println!("Player 0's public symbols:  {:?}", state.public_symbols [0]);
  println!("Player 0's private symbols: {:?}", state.private_symbols[0]);
  println!("Player 1's public symbols:  {:?}", state.public_symbols [1]);
  println!("Player 1's private symbols: {:?}", state.private_symbols[1]);

  let score = state.calc_score(0);
  println!("Final score: {:?}", score);

  match score.cmp(&0) {
    Ordering::Greater => println!("Player 0 wins!"),
    Ordering::Less    => println!("Player 1 wins!"),
    Ordering::Equal   => println!("It is a tie!"),
  }
}
