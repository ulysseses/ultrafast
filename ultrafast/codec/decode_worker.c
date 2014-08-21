#include "codec/worker.h"
#include "sizes.h"

extern const J_COLOR_SPACE PIX_FMT;

static JOCTET out_buffer[SCREEN_HEIGHT * SCREEN_WIDTH * NUM_CHANNELS];
static struct jpeg_decompress_struct	dinfo;
static struct jpeg_error_mgr			jerr;
static struct jpeg_source_mgr			smgr;

byte* decoder (byte *unprocessed_data, size_t size) {
	// NOTE: size is not used (we use sizes.h's h, w, & num_channels
	smgr.next_input_byte = (JOCTET)unprocessed_data;
	JSAMPROW row_pointer;  // also unsigned char *
	const size_t pitch = width * num_channels;
	jpeg_start_decompress(&dinfo);
	while (dinfo.output_scanline < dinfo.output_height) {
		row_pointer = (JSAMPROW)
			&out_buffer[dinfo.next_scanline * pitch];
		jpeg_read_scanlines(&dinfo, &row_pointer, 1);
	}
	jpeg_finish_decompress(&dinfo);
	return out_buffer;
}

int main (int argc, char * argv[]) {
	if (argc < 3) {
		printf("syntax: decoder_worker name");
		return 0;
	}
	char *fe_path = argv[1];
	char *be_path = argv[2];
	
	dinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&dinfo);
	smgr.init_source 		= init_buffer;  // buffer setup callback
	smgr.fill_input_buffer	= fill_buffer;  // what to do when buffer is empty
	smgr.term_source		= term_buffer;  // finalize buffer & clean-up callback
	//smgr.next_input_byte	  = in_buffer;  // input buffer (unsigned char or JOCTET)
	smgr.bytes_in_buffer = SCREEN_HEIGHT * SCREEN_WIDTH * NUM_CHANNELS;
	dinfo.src				= &smgr;
	dinfo.image_width		= SCREEN_WIDTH;
	dinfo.image_height		= SCREEN_HEIGHT;
	dinfo.output_components = NUM_CHANNELS;
	dinfo.out_color_space	= PIX_FMT;
	jpeg_read_header(&dinfo, 1);	
	
	zworker_boiler(decoder, fe_path, be_path);
	
	jpeg_destroy_decompress(&dinfo);
	return 0;
}




