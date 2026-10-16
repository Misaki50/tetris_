#include <Arduino.h>

const int W = 10;
const int H = 20;

const byte P[7][4][4] = {
  {{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}},
  {{1,1,0,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}},
  {{0,1,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},
  {{0,1,1,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}},
  {{1,1,0,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},
  {{1,0,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},
  {{0,0,1,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}}
};

byte board[H][W];
byte cur[4][4];
int px, py;
unsigned long lastFall = 0;

void cp(byte a[4][4], byte b[4][4]) {
  for (int r = 0; r < 4; r++)
    for (int c = 0; c < 4; c++)
      b[r][c] = a[r][c];
}

void rotate(byte s[4][4], byte d[4][4]) {
  for (int r = 0; r < 4; r++)
    for (int c = 0; c < 4; c++)
      d[c][3 - r] = s[r][c];
}

bool fits(byte s[4][4], int x, int y) {
  for (int r = 0; r < 4; r++)
    for (int c = 0; c < 4; c++) {
      if (!s[r][c]) continue;
      int nx = x + c;
      int ny = y + r;
      if (nx < 0 || nx >= W || ny >= H) return false;
      if (ny >= 0 && board[ny][nx]) return false;
    }
  return true;
}

void clearLines() {
  for (int y = H - 1; y >= 0; y--) {
    bool full = true;
    for (int x = 0; x < W; x++)
      if (!board[y][x]) full = false;
    if (full) {
      for (int k = y; k > 0; k--)
        for (int x = 0; x < W; x++)
          board[k][x] = board[k - 1][x];
      for (int x = 0; x < W; x++)
        board[0][x] = 0;
      y++;
    }
  }
}

void spawn() {
  int t = random(7);
  for (int r = 0; r < 4; r++)
    for (int c = 0; c < 4; c++)
      cur[r][c] = P[t][r][c];
  px = 3;
  py = 0;
  if (!fits(cur, px, py)) {
    for (int y = 0; y < H; y++)
      for (int x = 0; x < W; x++)
        board[y][x] = 0;
  }
}

void lock() {
  for (int r = 0; r < 4; r++)
    for (int c = 0; c < 4; c++)
      if (cur[r][c] && py + r >= 0)
        board[py + r][px + c] = 1;
  clearLines();
  spawn();
}

void draw() {
  for (int i = 0; i < 30; i++) Serial.println();
  for (int y = 0; y < H; y++) {
    for (int x = 0; x < W; x++) {
      bool f = board[y][x];
      int ry = y - py;
      int rx = x - px;
      if (ry >= 0 && ry < 4 && rx >= 0 && rx < 4 && cur[ry][rx]) f = 1;
      Serial.print(f ? "[]" : " .");
    }
    Serial.println();
  }
  Serial.println();
  Serial.println("w rotar  a izq  d der  s bajar");
}

void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(A0));
  spawn();
  draw();
}

void loop() {
  bool ch = false;

  while (Serial.available()) {
    char c = Serial.read();
    ch = true;
    if (c == 'a' && fits(cur, px - 1, py)) px--;
    else if (c == 'd' && fits(cur, px + 1, py)) px++;
    else if (c == 's' && fits(cur, px, py + 1)) py++;
    else if (c == 'w') {
      byte t[4][4];
      rotate(cur, t);
      if (fits(t, px, py)) cp(t, cur);
    }
  }

  if (millis() - lastFall >= 500) {
    lastFall = millis();
    ch = true;
    if (fits(cur, px, py + 1)) py++;
    else lock();
  }

  if (ch) draw();
}
