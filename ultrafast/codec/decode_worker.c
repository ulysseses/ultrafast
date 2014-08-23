#include "codec/worker.h"
#include "sizes.h"
#include "jpeglib.h"

static const J_COLOR_SPACE PIX_FMT = JCS_GRAYSCALE;

static JOCTET out_buffer[SCREEN_HEIGHT * SCREEN_WIDTH * NUM_CHANNEL];
static struct jpeg_decompress_struct	dinfo;
static struct jpeg_error_mgr			jerr;
static struct jpeg_source_mgr			smgr;

byte* decoder (byte *unprocessed_data, size_t *size) {
	jpeg_read_header(&dinfo, 1);
	smgr.next_input_byte = (JOCTET*)unprocessed_data;
	JSAMPROW row_pointer;  // also unsigned char *
	const size_t pitch = SCREEN_WIDTH * NUM_CHANNEL;
	jpeg_start_decompress(&dinfo);
	while (dinfo.output_scanline < dinfo.output_height) {
		row_pointer = (JSAMPROW)
			&out_buffer[dinfo.output_scanline * pitch];
		jpeg_read_scanlines(&dinfo, &row_pointer, 1);
	}
	jpeg_finish_decompress(&dinfo);

	*size = sizeof(out_buffer);
	return out_buffer;
}

void init_buffer(struct jpeg_decompress_struct *dinfo) {}
boolean fill_buffer(struct jpeg_decompress_struct *dinfo) { return 1; }  // from jpeglib.h
void term_buffer(struct jpeg_decompress_struct *dinfo) {}

// debug
#include <stdio.h>

int main (int argc, char * argv[]) {
	if (argc < 3) {
		printf("syntax: decode_worker frontend backend\n");
		return 0;
	}
	char *fe_path = argv[1];
	char *be_path = argv[2];
	
	/* create decompressor */
	jpeg_create_decompress(&dinfo);
	dinfo.err = jpeg_std_error(&jerr);
	dinfo.do_fancy_upsampling = FALSE;

	smgr.init_source 		= init_buffer;  // buffer setup callback
	smgr.fill_input_buffer	= fill_buffer;  // what to do when buffer is empty
	smgr.term_source		= term_buffer;  // finalize buffer & clean-up callback
	//smgr.next_input_byte	  = in_buffer;  // input buffer (unsigned char or JOCTET)
	smgr.bytes_in_buffer = SCREEN_HEIGHT * SCREEN_WIDTH * NUM_CHANNEL;
	dinfo.src				= &smgr;
	dinfo.image_width		= SCREEN_WIDTH;
	dinfo.image_height		= SCREEN_HEIGHT;
	dinfo.output_components = NUM_CHANNEL;
	dinfo.out_color_space	= PIX_FMT;
	zworker_boiler(decoder, fe_path, be_path);
	
	jpeg_destroy_decompress(&dinfo);
	return 0;
}




