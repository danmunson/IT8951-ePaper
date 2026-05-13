#include "../lib/Config/DEV_Config.h"
#include "../lib/e-Paper/EPD_IT8951.h"
#include "../lib/GUI/GUI_Paint.h"
#include "../lib/GUI/GUI_BMPfile.h"
#include "../lib/Config/Debug.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

static UBYTE *Frame_Buf = NULL;
static IT8951_Dev_Info Dev_Info = {0, 0};

extern UBYTE *bmp_src_buf;
extern UBYTE *bmp_dst_buf;

static void Cleanup_Exit(int signo)
{
    (void)signo;
    if (Frame_Buf != NULL) {
        free(Frame_Buf);
        Frame_Buf = NULL;
    }
    if (bmp_src_buf != NULL) {
        free(bmp_src_buf);
        bmp_src_buf = NULL;
    }
    if (bmp_dst_buf != NULL) {
        free(bmp_dst_buf);
        bmp_dst_buf = NULL;
    }
    if (Dev_Info.Panel_W != 0) {
        EPD_IT8951_Sleep();
    }
    DEV_Module_Exit();
    exit(0);
}

int main(int argc, char *argv[])
{
    signal(SIGINT, Cleanup_Exit);

    if (argc != 3) {
        printf("Usage: sudo ./push-file <VCOM> <path-to-bmp>\r\n");
        printf("Example: sudo ./push-file -2.51 ./pic/image.bmp\r\n");
        return 1;
    }

    double vcom_input;
    if (sscanf(argv[1], "%lf", &vcom_input) != 1) {
        printf("Invalid VCOM value: %s\r\n", argv[1]);
        return 1;
    }
    UWORD VCOM = (UWORD)(fabs(vcom_input) * 1000);
    const char *path = argv[2];

    Debug("VCOM value: %d\r\n", VCOM);

    if (DEV_Module_Init() != 0) {
        return -1;
    }

    Dev_Info = EPD_IT8951_Init(VCOM);

    UWORD Panel_Width = Dev_Info.Panel_W;
    UWORD Panel_Height = Dev_Info.Panel_H;
    UDOUBLE Target_Memory_Addr = Dev_Info.Memory_Addr_L | (Dev_Info.Memory_Addr_H << 16);

    bool Four_Byte_Align_Local = false;
    char *LUT_Version = (char *)Dev_Info.LUT_Version;
    if (strcmp(LUT_Version, "M641") == 0 || strcmp(LUT_Version, "M841_TFAB512") == 0) {
        Four_Byte_Align_Local = true;
    }

    EPD_IT8951_Clear_Refresh(Dev_Info, Target_Memory_Addr, INIT_Mode);

    UWORD WIDTH = Four_Byte_Align_Local ? (Panel_Width - (Panel_Width % 32)) : Panel_Width;
    UWORD HEIGHT = Panel_Height;
    UBYTE BitsPerPixel = 4;

    UDOUBLE Imagesize = ((WIDTH * BitsPerPixel % 8 == 0)
                        ? (WIDTH * BitsPerPixel / 8)
                        : (WIDTH * BitsPerPixel / 8 + 1)) * HEIGHT;

    if ((Frame_Buf = (UBYTE *)malloc(Imagesize)) == NULL) {
        Debug("Failed to allocate frame buffer\r\n");
        DEV_Module_Exit();
        return -1;
    }

    Paint_NewImage(Frame_Buf, WIDTH, HEIGHT, 0, BLACK);
    Paint_SelectImage(Frame_Buf);
    Paint_SetRotate(ROTATE_0);
    Paint_SetMirroring(MIRROR_NONE);
    Paint_SetBitsPerPixel(BitsPerPixel);
    Paint_Clear(WHITE);

    if (GUI_ReadBmp(path, 0, 0) != 0) {
        Debug("Failed to load BMP: %s\r\n", path);
        free(Frame_Buf);
        Frame_Buf = NULL;
        EPD_IT8951_Sleep();
        DEV_Module_Exit();
        return -1;
    }

    EPD_IT8951_4bp_Refresh(Frame_Buf, 0, 0, WIDTH, HEIGHT, false, Target_Memory_Addr, false);

    free(Frame_Buf);
    Frame_Buf = NULL;

    EPD_IT8951_Sleep();
    DEV_Delay_ms(100);
    DEV_Module_Exit();
    return 0;
}
