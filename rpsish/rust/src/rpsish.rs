extern crate rand;
extern crate num;

use self::rand::Rng;
use self::num::integer::Integer;
use Player;
use GameState;

#[derive(Debug)]
pub struct RpsishConfig {
  pub num_symbols: usize,
  pub num_rounds: usize,
  pub payoffs: Vec<Vec<i32>>,
}

#[derive(Debug)]
pub struct RpsishState<'a> {
  pub config: &'a RpsishConfig,
  pub player: usize,
  pub public_symbols: [Vec<usize>; 2],
  pub private_symbols: [Vec<usize>; 2],
}

#[derive(Debug)]
pub struct RpsishView<'a> {
  config: &'a RpsishConfig,
  public_symbols: [Vec<usize>; 2],
  my_private_symbols: Vec<usize>,
}

#[derive(Debug)]
pub struct RpsishAction {
  public_symbol: usize,
  private_symbol: usize,
}

impl<'a> GameState<'a> for RpsishState<'a> {
  type View = RpsishView<'a>;
  type Action = RpsishAction;

  fn done(&self) -> bool {
    let num_rounds = self.config.num_rounds;
    self.private_symbols.into_iter().all(|x| x.len() >= num_rounds)
  }

  fn apply_action(&mut self, action: &RpsishAction) {
    self.public_symbols [self.player].push(action.public_symbol);
    self.private_symbols[self.player].push(action.private_symbol);
    self.player = (self.player + 1) % 2;
  }

  fn to_view(&self, pid: usize) -> RpsishView<'a> {
    RpsishView {
      config: &self.config,
      public_symbols: self.public_symbols.clone(),
      my_private_symbols: self.private_symbols[pid].clone(),
    }
  }

  fn moves(&self) -> Vec<RpsishAction> {
    // TODO: implement
    vec![]
  }
}

fn modulo<T: Integer + Copy>(x: T, m: T) -> T {
  ((x % m) + m) % m
}

/* Players are types that implement the Player trait. RandomPlayer is a unit struct since it
 * doesn't need any state. But one could conceive of, say, a CfrPlayer that needs to be fed the
 * model resulting from a CFR training session. */
pub struct RandomPlayer;
impl<'a> Player<'a> for RandomPlayer {
  type Game = RpsishState<'a>;
  fn make_move(&self, _pid: usize, view: &RpsishView) -> RpsishAction {
    RpsishAction {
      public_symbol:  modulo(rand::thread_rng().gen::<usize>(), view.config.num_symbols),
      private_symbol: modulo(rand::thread_rng().gen::<usize>(), view.config.num_symbols),
    }
  }
}

// ConstantPlayer always plays a constant move.
pub struct ConstantPlayer {
  pub public_symbol: usize,
  pub private_symbol: usize,
}
impl<'a> Player<'a> for ConstantPlayer {
  type Game = RpsishState<'a>;
  fn make_move(&self, _pid: usize, _view: &RpsishView) -> RpsishAction {
    RpsishAction {
      public_symbol:  self.public_symbol,
      private_symbol: self.private_symbol,
    }
  }
}

// SimpletonPlayer looks at the other player's public symbols and makes the best move based on
// those, or 0 otherwise.
pub struct SimpletonPlayer;
impl<'a> Player<'a> for SimpletonPlayer {
  type Game = RpsishState<'a>;
  fn make_move(&self, pid: usize, view: &RpsishView) -> RpsishAction {
    let you = (pid + 1) % 2;
    let mut best: usize = 0;
    let mut best_score = -1;
    for i in 0..view.config.num_symbols {
      /* Why those ampersands precisely where they are? Because b o r r o w. */
      let &score =
        &view.public_symbols[you].iter()
        .map(|&j| view.config.payoffs[i][j])
        .sum();
      if score > best_score {
        best = i;
        best_score = score;
      }
    }
    RpsishAction {
      public_symbol:  best,
      private_symbol: best,
    }
  }
}

impl<'a> RpsishState<'a> {
  pub fn calc_score(&self, pid: usize) -> i32 {
    let you = (pid + 1) % 2;
    let mut p1 =   self.private_symbols[pid].clone();
    p1.append(&mut self.public_symbols [pid].clone());
    let mut p2 =   self.private_symbols[you].clone();
    p2.append(&mut self.public_symbols [you].clone());
    let mut score = 0;
    for &s in &p1 {
      for &t in &p2 {
        score += self.config.payoffs[s][t];
      }
    }
    score
  }
}

