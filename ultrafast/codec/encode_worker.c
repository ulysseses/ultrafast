#include "codec/worker.h"
#include "sizes.h"

extern const J_COLOR_SPACE PIX_FMT;

static JOCTET out_buffer[SCREEN_HEIGHT * SCREEN_WIDTH * NUM_CHANNELS];
static struct jpeg_compress_struct	cinfo;
static struct jpeg_error_mgr		jerr;
static struct jpeg_destination_mgr	dmgr;

byte* encoder (byte *unprocessed_data, size_t size) {
	// NOTE: size is not used (we use sizes.h's h, w, & num_channels
	JSAMPROW row_pointer;  // also unsigned char *
	const size_t pitch = width * num_channels;
	jpeg_start_compress(&cinfo);
	while (cinfo.next_scanline < cinfo.image_height) {
		row_pointer = (JSAMPROW)
			&unprocessed_data[cinfo.next_scanline * pitch];
		jpeg_write_scanlines(&cinfo, &row_pointer, 1);
	}
	jpeg_finish_compress(&cinfo);
	return out_buffer;
}

int main (int argc, char * argv[]) {
	if (argc < 3) {
		printf("syntax: encoder_worker name");
		return 0;
	}
	char *fe_path = argv[1];
	char *be_path = argv[2];
	
	cinfo.err = jpeg_std_error(&cinfo);
	jpeg_create_compress(&cinfo);
	dmgr.init_destination		= init_buffer;  // buffer setup callback
	dmgr.empty_output_buffer	= empty_buffer;  // buffer full callback
	dmgr.term_destination		= term_buffer;  // finalization & cleanup
	dmgr.next_output_byte		= out_buffer;  // output buffer (JOCTET)
	cinfo.dest				= &dmgr;
	cinfo.image_width		= SCREEN_WIDTH;
	cinfo.image_height		= SCREEN_HEIGHT;
	cinfo.input_components	= NUM_CHANNELS;
	cinfo.in_color_space	= PIX_FMT;
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, 75, 1);
	
	zworker_boiler(encoder, fe_path, be_path);
	
	jpeg_destroy_compress(&cinfo);
	return 0;
}




