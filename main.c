// implementation

#include "build/script.h"
#include "uyghur/uyghur.c"
#include "libraries/header.h"

// unsigned char __build_script_ug[] = {
//   0x23, 0x20, 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x77, 0x6f, 0x72, 0x6c,
//   0x64, 0x0a, 0x0a, 0x69, 0x6b, 0x72, 0x61, 0x6e, 0x67, 0x68, 0x61, 0x61,
//   0x20, 0x5b, 0x6d, 0x65, 0x72, 0x68, 0x61, 0x62, 0x61, 0x20, 0x75, 0x79,
//   0x67, 0x68, 0x75, 0x72, 0x20, 0x73, 0x63, 0x72, 0x69, 0x70, 0x74, 0x21,
//   0x5d, 0x20, 0x79, 0x65, 0x7a, 0x69, 0x6c, 0x73, 0x75, 0x6e, 0x0a
// };
// unsigned int __build_script_ug_len = 59;

int main(int argc, char const *argv[])
{
    //
    Uyghur *uyghur = Uyghur_new();
    register_external_libraries(uyghur->bridge);
    if (argc >= 2) Uyghur_execute(uyghur, (char *)argv[1]);
    if (argc == 1) Uyghur_run(uyghur, "script.ug", __build_script_ug);
    Uyghur_free(uyghur);
    //
    return 0;
}
