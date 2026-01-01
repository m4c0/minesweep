export module mainloop;

#ifdef LECO_TARGET_WASM
import vaselin;
#else
import mtx;
import silog;
import sith;
#endif

export namespace mainloop {
  using fn_t = void (*)(void);

#ifdef LECO_TARGET_WASM
  void push(fn_t fn) {
    vaselin::set_timeout([](void * ptr) {
      reinterpret_cast<fn_t>(ptr)();
    }, reinterpret_cast<void *>(fn), 0);
  }
#else
  void push(fn_t fn);
#endif
}

#ifndef LECO_TARGET_WASM
module : private;

static constexpr const auto queue_size = 128;

static mainloop::fn_t g_queue[queue_size] {};
static int g_next_prod = 0;
static int g_next_cons = 0;
static mtx::mutex g_mtx {};
static mtx::cond g_cond_cons {};
static mtx::cond g_cond_prod {};

static mainloop::fn_t take() {
  mtx::lock l { &g_mtx };
  if (g_next_prod == g_next_cons) {
    g_cond_cons.wait(&l, 1);
    if (g_next_prod == g_next_cons) return {};
  }

  auto res = g_queue[g_next_cons];
  g_next_cons = (g_next_cons + 1) % queue_size;
  g_cond_prod.wake_one();
  return res;
}
void mainloop::push(fn_t fn) {
  mtx::lock l { &g_mtx };

  auto p = (g_next_prod + 1) % queue_size;
  if (p == g_next_cons) {
    silog::warning("event thread queue is full - this might slow down the whole app");
    g_cond_prod.wait(&l, 1);
    if (p == g_next_cons) silog::die("event thread became too busy - bailing out");
  }

  g_queue[g_next_prod] = fn;
  g_next_prod = p;
  g_cond_cons.wake_one();
}

sith::stateless_thread g_thr { [](auto * t) {
  silog::info("Event loop started");
  while (!t->interrupted()) {
    // TODO: alarm if task takes too much time
    if (auto r = take()) r();
  }
} };
sith::run_guard g_thr_guard { &g_thr };

#endif
