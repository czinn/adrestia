use GameState;

use std::cmp::min;
use std::fmt;

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
  pub public_moves: [Vec<usize>; 2],
  pub private_moves: [Vec<usize>; 2],
}

#[derive(Debug)]
pub struct RpsishView<'a> {
  pub config: &'a RpsishConfig,
  pub public_moves: [Vec<usize>; 2],
  pub my_private_moves: Vec<usize>,
}

impl<'a> fmt::Display for RpsishState<'a> {
  fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
    write!(f, "{{ player: {}, public_moves: {:?}, private_moves: {:?} }}",
           self.player, self.public_moves, self.private_moves)
  }
}

impl<'a> fmt::Display for RpsishView<'a> {
  fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
    write!(f, "{{ public_moves: {:?}, my_private_moves: {:?} }}",
           self.public_moves, self.my_private_moves)
  }
}

#[derive(Debug)]
pub struct RpsishAction {
  pub public: usize,
  pub private: usize,
}

impl<'a> GameState<'a> for RpsishState<'a> {
  type View = RpsishView<'a>;
  type Action = RpsishAction;

  fn done(&self) -> bool {
    let num_rounds = self.config.num_rounds;
    self.private_moves.into_iter().all(|x| x.len() >= num_rounds)
  }

  fn apply_action(&mut self, action: &RpsishAction) {
    self.public_moves [self.player].push(action.public);
    self.private_moves[self.player].push(action.private);
    self.player = (self.player + 1) % 2;
  }

  fn to_view(&self, pid: usize) -> RpsishView<'a> {
    let n_revealed =
      self.public_moves.iter()
      .fold(usize::max_value(), |acc, ref s| min(acc, s.len()));
    let trunc = |s: &Vec<usize>| -> Vec<usize> {
      let mut result = s.clone();
      result.truncate(n_revealed);
      result
    };
    RpsishView {
      config: &self.config,
      public_moves: [trunc(&self.public_moves[0]), trunc(&self.public_moves[1])],
      my_private_moves: self.private_moves[pid].clone(),
    }
  }
}

impl<'a> RpsishState<'a> {
  pub fn calc_score(&self, pid: usize) -> i32 {
    let you = (pid + 1) % 2;
    let mut p1 =   self.private_moves[pid].clone();
    p1.append(&mut self.public_moves [pid].clone());
    let mut p2 =   self.private_moves[you].clone();
    p2.append(&mut self.public_moves [you].clone());
    let mut score = 0;
    for &s in &p1 {
      for &t in &p2 {
        score += self.config.payoffs[s][t];
      }
    }
    score
  }
}

