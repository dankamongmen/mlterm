/* -*- c-basic-offset:2; tab-width:2; indent-tabs-mode:nil -*- */

#include <stdio.h>
#include <pobl/bl_dlfcn.h>
#include <pobl/bl_str.h>
#include <pobl/bl_debug.h>
#include <pobl/bl_mem.h>

#include "zmodem.h"

#ifndef LIBDIR
#define TRANSFERLIB_DIR "/usr/local/lib/mlterm/"
#else
#define TRANSFERLIB_DIR LIBDIR "/mlterm/"
#endif

/* --- static variables --- */

static int zmodem_mode;
static struct file_info zmodem_info[2];

static void (*dl_zmodem)(unsigned char *, const unsigned int, unsigned char *, unsigned int *,
                      const unsigned int);
static Q_BOOL (*dl_zmodem_start)(struct file_info *, const char *, const Q_BOOL,
                                 const ZMODEM_FLAVOR);
static Q_BOOL (*dl_zmodem_is_processing)(int *);

static int is_tried;
static bl_dl_handle_t handle;

/* --- static functions --- */

static int load_library(void) {
  is_tried = 1;

  if (!(handle = bl_dl_open(TRANSFERLIB_DIR, "zmodem")) && !(handle = bl_dl_open("", "zmodem"))) {
    bl_error_printf("ZMODEM: Could not load.\n");

    return 0;
  }

  bl_dl_close_at_exit(handle);

  dl_zmodem = bl_dl_func_symbol(handle, "zmodem");
  dl_zmodem_start = bl_dl_func_symbol(handle, "zmodem_start");
  dl_zmodem_is_processing = bl_dl_func_symbol(handle, "zmodem_is_processing");

  return 1;
}

/* --- global functions --- */

int vt_transfer_start(/* vt_transfer_type_t type, */
                      char *send_file /* allocated by the caller */,
                      const char *save_dir, int is_crc32) {
  Q_BOOL ret;

  if (zmodem_mode || (is_tried ? (dl_zmodem_start == NULL) : !load_library())) {
    return 0;
  }

  if (send_file) {
    zmodem_info[0].name = send_file;
    stat(send_file, &zmodem_info[0].fstats);

    ret = (*dl_zmodem_start)(zmodem_info, save_dir, Q_TRUE, is_crc32 ? Z_CRC32 : Z_CRC16);
  } else {
    ret = (*dl_zmodem_start)(NULL, save_dir, Q_FALSE, is_crc32 ? Z_CRC32 : Z_CRC16);
  }

  if (ret) {
    zmodem_mode = 1;

    return 1;
  } else {
    return 0;
  }
}

void vt_transfer_data(u_char *input, const u_int input_n, u_char *output, u_int *output_n,
                      const u_int output_max) {
#if 0
  if (input_n > 0) {
    FILE *fp = fopen("zmodem.log", "a");
    u_int i;

    fprintf(fp, "INPUT %d\n", input_n);
    for (i = 0; i < input_n; i++) {
      fprintf(fp, "%c", input[i]);
    }
    fprintf(fp, "\n");
    fclose(fp);
  }
#endif

  (*dl_zmodem)(input, input_n, output, output_n, output_max);

#if 0
  if (*output_n > 0) {
    FILE *fp = fopen("zmodem.log", "a");
    u_int i;

    fprintf(fp, "OUTPUT %d\n", *output_n);
    for (i = 0; i < *output_n; i++) {
      fprintf(fp, "%c", output[i]);
    }
    fprintf(fp, "\n");
    fclose(fp);
  }
#endif
}

int vt_transfer_is_processing(int *progress) {
  static int prev_progress = -1;

  if (zmodem_mode) {
    if ((*dl_zmodem_is_processing)(progress)) {
      if (prev_progress < *progress) {
        prev_progress = *progress;

        return 2;
      } else {
        return -1;
      }
    } else {
      zmodem_mode = 0;
      free(zmodem_info[0].name);
      zmodem_info[0].name = NULL;
      prev_progress = -1;

      return 1;
    }
  }

  return 0;
}
