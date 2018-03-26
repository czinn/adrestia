extern crate rand;
extern crate num;

use Player;

use self::rand::Rng;
use self::num::integer::Integer;
use rpsish::*;

fn modulo<T: Integer + Copy>(x: T, m: T) -> T {
  ((x % m) + m) % m
}

/* Players are types that implement the Player trait. RandomPlayer is a unit struct since it
 * doesn't need any state. But one could conceive of, say, a CfrPlayer that needs to be fed the
 * model resulting from a CFR training session. */
pub struct RandomPlayer;
impl<'a> Player<'a> for RandomPlayer {
  type Game = RpsishState<'a>;
  fn pick_move(&self, _pid: usize, view: &RpsishView) -> RpsishAction {
    RpsishAction {
      public:  modulo(rand::thread_rng().gen::<usize>(), view.config.num_symbols),
      private: modulo(rand::thread_rng().gen::<usize>(), view.config.num_symbols),
    }
  }
}

// ConstantPlayer always plays a constant move.
pub struct ConstantPlayer {
  pub public: usize,
  pub private: usize,
}
impl<'a> Player<'a> for ConstantPlayer {
  type Game = RpsishState<'a>;
  fn pick_move(&self, _pid: usize, _view: &RpsishView) -> RpsishAction {
    RpsishAction {
      public:  self.public,
      private: self.private,
    }
  }
}

// SimpletonPlayer looks at the other player's public symbols and makes a best move based on those.
pub struct SimpletonPlayer;
impl<'a> Player<'a> for SimpletonPlayer {
  type Game = RpsishState<'a>;
  fn pick_move(&self, pid: usize, view: &RpsishView) -> RpsishAction {
    let you = (pid + 1) % 2;
    let mut best: usize = 0;
    let mut best_score = -1;
    for i in 0..view.config.num_symbols {
      /* Why those ampersands precisely where they are? Because b o r r o w. */
      let &score =
        &view.public_moves[you].iter()
        .map(|&j| view.config.payoffs[i][j])
        .sum();
      if score > best_score {
        best = i;
        best_score = score;
      }
    }
    RpsishAction {
      public:  best,
      private: best,
    }
  }
}

