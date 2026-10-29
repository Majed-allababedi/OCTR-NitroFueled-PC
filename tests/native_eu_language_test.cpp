// Synthetic in-memory structures only; includes the actual hook implementation.
#include "../gpu/shim/bbport_graphics_tab.cpp"
#include <array>
#include <cassert>
extern "C" { int runtime_graphics_tab_ready = 0; void runtime_restart() { assert(false); } }
namespace BbOverlay { void QuitGame() { assert(false); } }
namespace BbSettings {
Values& Get() { static Values value; return value; }
void Save() { assert(false); }
bool UltrawideAvailable() { return false; }
void SetUltrawide(bool) { assert(false); }
bool ResolutionNeedsRestart() { return false; }
bool RenderSizeNeedsRestart() { return false; }
}
static int getters, releases, forwards;
static bool expect_held;
static void* target;
static void* expected[6];
static void PS4_SYSV_ABI getter(void*, void*, void** out) { ++getters; *out = target; }
static void PS4_SYSV_ABI release(void* p) { assert(p == target); ++releases; }
static std::uint64_t PS4_SYSV_ABI original(void* a, void* b, void* c, void* d, void* e, void* f) {
    assert(a==expected[0] && b==expected[1] && c==expected[2] && d==expected[3] && e==expected[4] && f==expected[5]);
    assert(releases == getters - (expect_held ? 1 : 0)); ++forwards; return 0x12345678;
}
int main() {
    using namespace BbGraphicsTab;
    auto store = [](void* p, std::uintptr_t v) { std::memcpy(p,&v,8); };
    auto put32 = [](void* p, std::uint32_t v) { std::memcpy(p,&v,4); };
    alignas(8) std::array<unsigned char,0x5880> pool{};
    alignas(8) std::array<unsigned char,0xa0> owner{};
    alignas(8) std::array<std::array<unsigned char,0x120>,4> nodes{};
    auto base=reinterpret_cast<std::uintptr_t>(pool.data());
    auto op=reinterpret_cast<std::uintptr_t>(owner.data());
    auto node=[&](int i){return reinterpret_cast<std::uintptr_t>(nodes[i].data());};
    put32(pool.data()+0x10,72); put32(pool.data()+0x53e8,72);
    const unsigned indices[]={0,1,2,38}, objects[]={0x268,0x280,0x298,0x1370};
    const std::uint32_t hashes[]={0x58158328,0xa166eff7,0x0e41d344,0xe9d89d08};
    for(int i=0;i<4;++i) { store(pool.data()+0x28+8*indices[i],base+objects[i]); put32(pool.data()+0x5408+16*indices[i],hashes[i]); }
    const unsigned templates[]={0x1370,0xaf0,0x4e0,0x4ff0};
    for(int i=0;i<4;++i) { store(nodes[i].data()+0x18,base+templates[i]); store(nodes[i].data()+0x20,op); store(nodes[i].data()+0x40,i<3?node(i+1):0); }
    store(pool.data()+0x1370+0x40,base+0xaf0); store(pool.data()+0xaf0+0x40,base+0x4e0);
    store(owner.data()+0x98,base+0x4f00); store(owner.data()+0x10,node(3));
    for(auto p : {std::pair{0x10,0xf0}, {0x18,0x210}, {0x38,0x430}, {0xa0,0x470}}) store(pool.data()+0x4f00+p.first,base+0x4f00+p.second);
    void* pins[]={nodes[0].data()}; alignas(8) unsigned char args[0x18]{}; store(args+0x10,reinterpret_cast<std::uintptr_t>(pins));
    expected[0]=owner.data(); expected[1]=args; expected[2]=nodes[0].data(); expected[3]=nodes[1].data(); expected[4]=nodes[2].data(); expected[5]=nodes[3].data();
    image=reinterpret_cast<u8*>(reinterpret_cast<std::uintptr_t>(&getter)-PinGetter);
    // Place both stub entry points at their real image-relative locations using executable trampolines in the driver.
    extern void setup_eu_language_image(); setup_eu_language_image();
    code_shift=0x10; hide_original=show_original=open2_original=language_item_original=&original;
    target=nodes[0].data();
    auto call=[&](VscFn fn){return fn(expected[0],expected[1],expected[2],expected[3],expected[4],expected[5]);};
    assert(call(HideHook)==0 && getters==1 && releases==1 && forwards==0);
    auto reject=[&]{int before=forwards; expect_held=true; assert(call(HideHook)==0x12345678); expect_held=false; assert(forwards==before+1 && getters==releases);};
    store(pool.data()+0x28+8*42,base+0x1250); put32(pool.data()+0x5408+16*42,0xecd8a1c1);
    store(pool.data()+0x1250+0x40,base+0xaf0);
    store(nodes[0].data()+0x18,base+0x1250); reject(); store(nodes[0].data()+0x18,base+0x1370);
    store(nodes[0].data()+0x18,base+0x1200); reject(); store(nodes[0].data()+0x18,base+0x1370);
    store(nodes[1].data()+0x20,node(0)); reject(); store(nodes[1].data()+0x20,op);
    store(owner.data()+0x10,node(2)); reject(); store(owner.data()+0x10,node(3));
    store(owner.data()+0x98,base+0x4f08); reject(); store(owner.data()+0x98,base+0x4f00);
    store(nodes[1].data()+0x40,node(0)); reject(); store(nodes[1].data()+0x40,node(2));
    store(nodes[0].data()+0x40,0x10000); reject(); store(nodes[0].data()+0x40,node(1)+1); reject(); store(nodes[0].data()+0x40,node(1));
    put32(pool.data()+0x5408+16*38,0x4dd60b28); reject(); put32(pool.data()+0x5408+16*38,0xe9d89d08);
    pool[0x5408]^=1; reject(); pool[0x5408]^=1;
    int before=getters; assert(call(ShowHook)==0x12345678 && getters==before);
    assert(call(Open2Hook)==0x12345678 && getters==before);
    expected[0]=reinterpret_cast<void*>(1); expected[1]=reinterpret_cast<void*>(1);
    assert(call(LanguageItemHook)==0x12345678 && getters==before);
    expected[0]=nullptr; expected[1]=nullptr;
    assert(call(LanguageItemHook)==0x12345678 && call(Open2Hook)==0x12345678 && getters==before);
    expected[0]=owner.data(); expected[1]=args;
    code_shift=0;
    auto trampoline=[](u8* p, void* fn) { p[0]=0x48; p[1]=0xb8; auto v=reinterpret_cast<std::uintptr_t>(fn); std::memcpy(p+2,&v,8); p[10]=0xff; p[11]=0xe0; };
    trampoline(image+Release,reinterpret_cast<void*>(&release));
    expect_held=true; assert(call(HideHook)==0x12345678); expect_held=false;
    assert(getters==releases);
    std::puts("EU language native synthetic: PASS");
}
#include <sys/mman.h>
void setup_eu_language_image() {
    using namespace BbGraphicsTab;
    constexpr size_t size=Release+4096;
    void* p=mmap(nullptr,size,PROT_READ|PROT_WRITE|PROT_EXEC,MAP_PRIVATE|MAP_ANONYMOUS,-1,0); assert(p!=MAP_FAILED);
    image=static_cast<u8*>(p);
    auto trampoline=[](u8* p, void* fn) { p[0]=0x48; p[1]=0xb8; auto v=reinterpret_cast<std::uintptr_t>(fn); std::memcpy(p+2,&v,8); p[10]=0xff; p[11]=0xe0; };
    trampoline(image+PinGetter,reinterpret_cast<void*>(&getter)); trampoline(image+Release-0x10,reinterpret_cast<void*>(&release));
}
