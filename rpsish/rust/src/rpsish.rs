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
  pub config: &'a RpsishConfig,
  pub public_symbols: [Vec<usize>; 2],
  pub my_private_symbols: Vec<usize>,
}

#[derive(Debug)]
pub struct RpsishAction {
  pub public_symbol: usize,
  pub private_symbol: usize,
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

