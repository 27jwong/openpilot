#include "common/tests/native_test.h"
#include "common/hardware/hw.h"
#include "tools/replay/replay.h"
#include "tools/replay/route.h"
#include "tools/replay/filereader.h"
#include <filesystem>
#include <fstream>
#include <memory>
#include <fcntl.h>
#include <random>
#include <sys/file.h>
#ifdef __linux__
#include <sys/resource.h>
#include <sys/wait.h>
#endif
#include <unistd.h>
#include <zstd.h>

void test_route_ranges() {
  struct Range {
    const char *suffix;
    int begin;
    int end;
  };
  const Range ranges[] = {
    {"", 0, -1},
    {"/:15", 0, 15},
    {"/:0", 0, 0},
    {"/0:15", 0, 15},
    {"/5:15", 5, 15},
    {"/5:", 5, -1},
    {"/5", 5, 5},
    {"--5", 5, -1},
  };
  for (const std::string route : {DEMO_ROUTE, "64e16c4237493597/00000033--332a5f9d82"}) {
    const auto full = Route::parseRoute(route);
    REQUIRE(!full.str.empty());
    for (const auto &range : ranges) {
      const auto parsed = Route::parseRoute(route + range.suffix);
      REQUIRE(parsed.str == full.str);
      REQUIRE(parsed.dongle_id == full.dongle_id);
      REQUIRE(parsed.timestamp == full.timestamp);
      REQUIRE(parsed.begin_segment == range.begin);
      REQUIRE(parsed.end_segment == range.end);
    }
  }
}

void test_replay_recorder_only_service() {
  Replay all_services(DEMO_ROUTE, {}, {});
  Replay recorder_service(DEMO_ROUTE, {"modelIdentity", "logMessage"}, {});
}

void test_local_sources() {
  char directory[] = "/tmp/replay-route-XXXXXX";
  const std::filesystem::path root = mkdtemp(directory);
  const std::string route = "00000001--1234567890";
  const auto segment = root / (route + "--0"), collision = root / ("extra" + route + "--0");
  std::filesystem::create_directory(collision);
  std::ofstream(collision / "rlog") << "collision";
  CHECK(!Route(route, root, false, true).load());
  std::filesystem::create_directory(segment);
  std::ofstream(segment / "rlog") << "original";
  {
    Route local(route, root, false, true);
    REQUIRE(local.load());
    CHECK(local.segments().size() == 1);
    const auto &files = local.segments().begin()->second;
    auto held = files.openLocal(files.rlog);
    REQUIRE(held);
    unique_fd other(open(segment.c_str(), O_RDONLY | O_DIRECTORY));
    CHECK(flock(other, LOCK_EX | LOCK_NB) != 0);
    std::filesystem::remove(segment / "rlog");
    std::ofstream(segment / "rlog") << "replaced";
    CHECK(!files.openLocal(files.rlog));
    char bytes[8] = {};
    CHECK(pread(*held, bytes, sizeof(bytes), 0) == sizeof(bytes));
    CHECK(std::string(bytes, sizeof(bytes)) == "original");
  }
  std::ofstream(segment / "rlog.lock");
  CHECK(!Route(route, root, false, true).load());
  std::filesystem::remove(segment / "rlog.lock");
  std::filesystem::remove(segment / "rlog");
  std::filesystem::create_symlink(collision / "rlog", segment / "rlog");
  CHECK(!Route(route, root, false, true).load());
  std::filesystem::remove_all(root);
}

void test_machine_camera_time() {
  capnp::MallocMessageBuilder builder;
  auto event = builder.initRoot<cereal::Event>();
  event.setLogMonoTime(10050000000ULL);  // Recorded index arrives after camera EOF.
  auto index = event.initNarrowRoadEncodeIdx();
  index.setType(cereal::EncodeIndex::Type::FULL_H_E_V_C);
  index.setTimestampSof(10000000000ULL);
  index.setTimestampEof(10010000000ULL);
  index.setFrameId(7);
  index.setSegmentNum(0);
  auto words = capnp::messageToFlatArray(builder);
  auto bytes = words.asBytes();
  LogReader reader({}, 128 * 1024 * 1024, 3000000000LL);
  REQUIRE(reader.load(reinterpret_cast<const char *>(bytes.begin()), bytes.size()));
  REQUIRE(reader.events.size() == 2);
  CHECK(reader.events[0].eidx_segnum == -1);
  CHECK(reader.events[1].eidx_segnum == 0);
  CHECK(reader.events[0].mono_time == 7050000000ULL);
  CHECK(reader.events[1].mono_time == reader.events[0].mono_time);
  capnp::FlatArrayMessageReader raw(reader.events[1].data);
  CHECK(raw.getRoot<cereal::Event>().getLogMonoTime() == 10050000000ULL);
  CHECK(raw.getRoot<cereal::Event>().getNarrowRoadEncodeIdx().getTimestampSof() == 10000000000ULL);
}

void test_bounded_file_reader() {
  char path[] = "/tmp/replay-reader-XXXXXX";
  unique_fd fd(mkstemp(path));
  REQUIRE(fd >= 0);
  const std::string data(128 * 1024 + 17, '\0');
  REQUIRE(write(fd, data.data(), data.size()) == data.size());
  CHECK(FileReader(false, data.size()).read(path) == data);
  CHECK(FileReader(false, data.size() - 1).read(path).empty());
  CHECK(FileReader(false).read(path) == data);
  std::atomic<bool> abort = true;
  CHECK(FileReader(false, data.size()).read(path, &abort).empty());
  REQUIRE(unlink(path) == 0);
#ifdef __linux__
  CHECK(FileReader(false, data.size()).read("/proc/self/fd/" + std::to_string(int(fd))) == data);
#endif
  CHECK(FileReader(false, data.size()).read(path).empty());
}

void test_bounded_zstd_file_reader() {
  char path[] = "/tmp/replay-zstd-reader-XXXXXX";
  unique_fd fd(mkstemp(path));
  REQUIRE(fd >= 0);
  const std::string data(128 * 1024 + 17, '\x17');
  auto compress = [](const std::string &value) {
    std::unique_ptr<ZSTD_CCtx, decltype(&ZSTD_freeCCtx)> context(ZSTD_createCCtx(), ZSTD_freeCCtx);
    REQUIRE(context);
    REQUIRE(!ZSTD_isError(ZSTD_CCtx_setParameter(context.get(), ZSTD_c_compressionLevel, 1)));
    REQUIRE(!ZSTD_isError(ZSTD_CCtx_setParameter(context.get(), ZSTD_c_contentSizeFlag, 0)));
    REQUIRE(!ZSTD_isError(ZSTD_CCtx_setParameter(context.get(), ZSTD_c_checksumFlag, 1)));
    std::string result(ZSTD_compressBound(value.size()), '\0');
    const size_t size = ZSTD_compress2(context.get(), result.data(), result.size(), value.data(), value.size());
    REQUIRE(!ZSTD_isError(size));
    result.resize(size);
    return result;
  };
  const std::string compressed = compress(data);
  REQUIRE(ZSTD_getFrameContentSize(compressed.data(), compressed.size()) == ZSTD_CONTENTSIZE_UNKNOWN);
  auto store = [&](const std::string &bytes) {
    std::ofstream output(path, std::ios::binary);
    output.write(bytes.data(), bytes.size());
    REQUIRE(output.good());
  };
  store(compressed);
  // Local Zstd reads must not need a Python executable or decoded temporary file.
  const std::string python_path = getenv("PATH") ? getenv("PATH") : "";
  REQUIRE(setenv("PATH", "", 1) == 0);
  CHECK(FileReader(false, data.size()).read(path) == data);
  REQUIRE(setenv("PATH", python_path.c_str(), 1) == 0);
  CHECK(FileReader(false, data.size() - 1).read(path).empty());
  CHECK(FileReader(false).read(path) == data);
  std::atomic<bool> abort = true;
  CHECK(FileReader(false, data.size()).read(path, &abort).empty());
  REQUIRE(unlink(path) == 0);
#ifdef __linux__
  store("replacement");
  CHECK(FileReader(false, data.size()).read("/proc/self/fd/" + std::to_string(int(fd))) == data);
#endif
  store(compressed.substr(0, compressed.size() - 1));
  CHECK(FileReader(false, data.size()).read(path).empty());
  store(compressed + "junk");
  CHECK(FileReader(false, data.size()).read(path).empty());
  store(compressed + compressed);
  CHECK(FileReader(false, data.size() * 2).read(path) == data + data);
  CHECK(FileReader(false, data.size() * 2 - 1).read(path).empty());
  std::string corrupt = compressed;
  corrupt.back() ^= 1;
  store(corrupt);
  CHECK(FileReader(false, data.size()).read(path).empty());
  store(compressed.substr(0, 4));
  CHECK(FileReader(false).read(path).empty());
  // A complete input chunk must still drain the decoder's buffered output.
  std::minstd_rand random(73);
  std::string noise(65536, '\0');
  for (char &byte : noise) byte = random() & 255;
  bool exact_chunk = false;
  for (size_t size = 65500; size <= noise.size(); ++size) {
    const auto value = noise.substr(0, size), frame = compress(value);
    if (frame.size() == 65536) {
      store(frame);
      CHECK(FileReader(false, value.size()).read(path) == value);
      exact_chunk = true;
      break;
    }
  }
  REQUIRE(exact_chunk);
  const std::string skippable("\x50\x2a\x4d\x18\x05\0\0\0hello", 13);
  store(compressed + skippable);
  CHECK(FileReader(false, data.size()).read(path) == data);
  auto dictionary = compressed;
  dictionary[4] |= 1;  // Frame declares a dictionary which this reader does not have.
  dictionary.insert(6, 1, '\1');
  store(dictionary);
  CHECK(FileReader(false).read(path).empty());
  auto window = compressed;
  window[5] = '\x90';  // A 256 MiB window exceeds the decoder's existing default.
  store(window);
  CHECK(FileReader(false).read(path).empty());
#ifdef __linux__
  auto constrained = [&](auto check) {
    const pid_t child = fork();
    REQUIRE(child >= 0);
    if (child == 0) {
      _exit(run_native_test([&]() {
        size_t pages = 0;
        std::ifstream("/proc/self/statm") >> pages;
        REQUIRE(pages > 0);
        // Force the cap reservation to fail without pressuring the host's RAM.
        const rlim_t bytes = pages * sysconf(_SC_PAGESIZE) + 8 * 1024 * 1024;
        const rlimit limit{bytes, bytes};
        REQUIRE(setrlimit(RLIMIT_AS, &limit) == 0);
        check();
      }));
    }
    int status = 0;
    REQUIRE(waitpid(child, &status, 0) == child);
    CHECK(WIFEXITED(status) && WEXITSTATUS(status) == 0);
  };
  store(compressed);
  constrained([&]() { CHECK(FileReader(false, 128 * 1024 * 1024).read(path) == data); });
  store(corrupt);
  constrained([&]() { CHECK(FileReader(false, 128 * 1024 * 1024).read(path).empty()); });
  store(compress(std::string(24 * 1024 * 1024, '\x17')));
  constrained([&]() { CHECK(FileReader(false, 128 * 1024 * 1024).read(path).empty()); });
#endif
  REQUIRE(unlink(path) == 0);
}

class ThrowingVideoDecoder : public VideoDecoder {
public:
  bool open(AVCodecParameters *, bool) override { return true; }
  bool decode(FrameReader *reader, int, VisionBuf *) override {
    active_reader_ = reader;
    throw std::runtime_error("injected decoder failure");
  }
};

void test_decoder_exception_is_a_failed_frame() {
  FrameReader reader;
  reader.width = reader.height = 2;
  reader.packets_info.push_back({0, 0});
  reader.decoder_ = std::make_shared<ThrowingVideoDecoder>();
  VisionBuf buffer;
  buffer.width = buffer.height = buffer.stride = 2;
  CHECK(!reader.get(0, &buffer));
  CHECK(reader.decoder_->active_reader_ == nullptr);
}

void test_decoder_reader_switch(const char *first, const char *second, bool no_hw_decoder) {
  FrameReader a, b;
  REQUIRE(a.loadFromFile(NarrowRoadCam, first, no_hw_decoder));
  REQUIRE(b.loadFromFile(NarrowRoadCam, second, no_hw_decoder));
  REQUIRE(a.decoder_ == b.decoder_);
#ifndef __APPLE__
  if (!no_hw_decoder && !Hardware::PC()) {
    REQUIRE(dynamic_cast<V4LVideoDecoder *>(a.decoder_.get()) != nullptr);
  }
#endif
  REQUIRE(a.width == b.width && a.height == b.height);
  std::vector<uint8_t> pixels(a.width * a.height * 3 / 2);
  VisionBuf buffer;
  buffer.width = a.width;
  buffer.height = a.height;
  buffer.stride = a.width;
  buffer.y = pixels.data();
  buffer.uv = pixels.data() + a.width * a.height;
  auto check = [&](FrameReader &reader, int index, uint8_t expected) {
    REQUIRE(reader.get(index, &buffer));
    CHECK(std::all_of(pixels.begin(), pixels.begin() + a.width * a.height, [expected](auto value) { return value == expected; }));
  };
  for (int iteration = 0; iteration < (no_hw_decoder ? 1 : 12); ++iteration) {
    check(a, 0, 30);
    check(b, 0, 200);
    check(a, 1, 90);
    check(a, 2, 120);
    check(b, 0, 200);
    check(b, 1, 220);
    check(b, 2, 240);
    check(a, 0, 30);
    check(a, 2, 120);
    check(a, 0, 30);
#ifndef __APPLE__
    if (!no_hw_decoder) {
      size_t descriptors = std::distance(std::filesystem::directory_iterator("/proc/self/fd"), std::filesystem::directory_iterator());
      static size_t warmed_descriptors = 0;
      if (iteration == 0) warmed_descriptors = descriptors;
      CHECK(descriptors == warmed_descriptors);
    }
#endif
  }
  std::weak_ptr<VideoDecoder> inactive;
  {
    FrameReader transient;
    REQUIRE(transient.loadFromFile(WideRoadCam, first, true));
    inactive = transient.decoder_;
  }
  CHECK(inactive.expired());
}

int main(int argc, char **argv) {
  return run_native_test([&]() {
    test_route_ranges();
    test_replay_recorder_only_service();
    test_local_sources();
    test_machine_camera_time();
    test_bounded_file_reader();
    test_bounded_zstd_file_reader();
    test_decoder_exception_is_a_failed_frame();
    if (argc == 3 || argc == 4) {
      REQUIRE(argc == 3 || std::string(argv[3]) == "--hardware");
      test_decoder_reader_switch(argv[1], argv[2], argc == 3);
    }
  });
}
