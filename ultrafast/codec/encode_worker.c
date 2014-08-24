#include "codec/worker.h"
#include "sizes.h"
#include "jpeglib.h"

static const J_COLOR_SPACE PIX_FMT = JCS_GRAYSCALE;

static JOCTET out_buffer[SCREEN_HEIGHT * SCREEN_WIDTH * NUM_CHANNEL];
static struct jpeg_compress_struct	cinfo;
static struct jpeg_error_mgr		jerr;
static struct jpeg_destination_mgr	dmgr;

/*
inline byte* encoder(byte *unprocessed_data, size_t size) {
	// identity
	byte *processed_data = (byte*) malloc(size);
	memcpy(processed_data, unprocessed_data, size);
	//free(unprocessed_data);
	return processed_data;
}
*/

byte* encoder (byte *unprocessed_data, size_t *size) {
	// NOTE: size is not used (we use sizes.h's h, w, & num_channels
	JSAMPROW row_pointer;  // also unsigned char *
	const size_t pitch = SCREEN_WIDTH * NUM_CHANNEL;
	jpeg_start_compress(&cinfo, 1);
	while (cinfo.next_scanline < cinfo.image_height) {
		row_pointer = (JSAMPROW)
			&unprocessed_data[cinfo.next_scanline * pitch];
		jpeg_write_scanlines(&cinfo, &row_pointer, 1);
	}
	jpeg_finish_compress(&cinfo);
	*size = sizeof(out_buffer);
	return out_buffer;
}

void init_buffer(struct jpeg_compress_struct *cinfo) {}
boolean empty_buffer(struct jpeg_compress_struct *cinfo) { return 1; }  // boolean is from jpeglib.h
void term_buffer(struct jpeg_compress_struct *cinfo) {}
int main (int argc, char * argv[]) {
	if (argc < 3) {
		printf("syntax: encode_worker frontend backend\n");
		return 0;
	}
	char *fe_path = argv[1];
	char *be_path = argv[2];
	
	jpeg_create_compress(&cinfo);
	cinfo.err = jpeg_std_error(&jerr);
	dmgr.init_destination		= init_buffer;  // buffer setup callback
	dmgr.empty_output_buffer	= empty_buffer;  // buffer full callback
	dmgr.term_destination		= term_buffer;  // finalization & cleanup
	dmgr.next_output_byte		= out_buffer;  // output buffer (JOCTET)
	cinfo.dest				= &dmgr;
	cinfo.image_width		= SCREEN_WIDTH;
	cinfo.image_height		= SCREEN_HEIGHT;
	cinfo.input_components	= NUM_CHANNEL;
	cinfo.in_color_space	= PIX_FMT;
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, 75, 1);
	
	zworker_boiler(encoder, fe_path, be_path);
	
	jpeg_destroy_compress(&cinfo);
	return 0;
}




