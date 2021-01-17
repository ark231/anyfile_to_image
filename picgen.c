#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<math.h>
#include<getopt.h>
#include<string.h>
#include<stdbool.h>

#define APPNAME "picgen"
#define APPSPAC "      "
#define BITWIDTH_CHAR 8
#define EXTENSION_MONO  "pbm"
#define EXTENSION_GRAY  "pgm"
#define EXTENSION_COLOR "ppm"
typedef unsigned long long ulonglong;
typedef enum{
	MONO,
	GRAY,
	COLOR
}color_mode;
char* add_extension(const char*,color_mode);
int main(int argc,char *argv[]){
	FILE *infile = NULL,*outfile = NULL;
	double X_ratio = 1.0;
	double Y_ratio = 1.0;
	color_mode output_mode = COLOR;
	bool help_required = false;
	bool no_extension = false;
	char *extension;
	struct option longopts[]={
		{"X_ratio"     ,required_argument,0,'X'},
		{"Y_ratio"     ,required_argument,0,'Y'},
		{"mode"        ,required_argument,0,'m'},
		{"help"        ,no_argument      ,0,'h'},//flagにはintが入れられて、_Boolとはなじまないので
		{"no_extension",no_argument      ,0,'n'},//flagにはintが入れられて、_Boolとはなじまないので
		{0        ,0                ,0, 0 },
	};
	int idx_lngopt=0;
	int val_opt=0;
	while((val_opt=getopt_long(argc,argv,"X:Y:m:h",longopts,&idx_lngopt))!=-1){
		char *err=NULL;
		switch(val_opt){
			case 'X':
				{
				double X_ratio_tmp=(double)strtod(optarg,&err);
				if(*err != '\n' && *err != '\0'){
					fprintf(stderr,"error: invalid value %s\n",optarg);
				}else{
					X_ratio=X_ratio_tmp;
				}
				break;
				 }
			case 'Y':
				{
				double Y_ratio_tmp=(double)strtod(optarg,&err);
				if(*err != '\n' && *err != '\0'){
					fprintf(stderr,"error: invalid value %s\n",optarg);
				}else{
					Y_ratio=Y_ratio_tmp;
				}
				break;
				}
			case 'm':
				if(strcmp(optarg,"mono")==0 || strcmp(optarg,"MONO")==0){
					output_mode=MONO;
				}else if(strcmp(optarg,"gray")==0 || strcmp(optarg,"GRAY")==0){
					output_mode=GRAY;
				}else if(strcmp(optarg,"color")==0 || strcmp(optarg,"COLOR")==0){
					output_mode=COLOR;
				}else{
					fprintf(stderr,"error: invalid value %s\n",optarg);
				}
				break;
			case 'h':
				help_required = true;
				break;
			case 'n':
				no_extension = true;
				break;
			case '?':
				//printf("error\n");
				break;
		}
	}
	if(help_required){
		printf(APPNAME" [options] input_filepath output_filepath                 \n");
		printf(APPSPAC" options                                                  \n");
		printf(APPSPAC" -h, --help                    :show this help and exit   \n");
		printf(APPSPAC" -X, --X_ratio NUM             :x ratio of output image   \n");
		printf(APPSPAC" -Y, --Y_ratio NUM             :y ratio of output image   \n");
		printf(APPSPAC" -m, --mode (MONO|GRAY|COLOR)  :color mode of output image\n");
		printf(APPSPAC" --no_extension                :do not add extension      \n");
		exit(EXIT_SUCCESS);
	}
	if(optind > argc-2){
		fprintf(stderr,"error: input or output file was not specified\n");
		exit(EXIT_FAILURE);
	}

	if((infile=fopen(argv[optind],"r"))==NULL){
		fprintf(stderr,"can't open input file\n");
		exit(EXIT_FAILURE);
	}
	struct stat input_status;
	if((stat(argv[optind],&input_status))!=0){
		fprintf(stderr,"can't get status of input file\n");
		exit(EXIT_FAILURE);
	}
	ulonglong input_filesize = (ulonglong)input_status.st_size;
	optind++;
	char *outfilename;
	if(no_extension){
		outfilename=argv[optind];
	}else{
		outfilename=add_extension(argv[optind],output_mode);
		if(outfilename==NULL){
			fprintf(stderr,"failed to add extension to filename\n");//そのまま拡張子無しで続行という手もあるが、もし拡張子を期待していて、拡張子なしの大事なファイルがあったとしたら、よきせぬ上書きにつながるので避けるべきであろう。
			exit(EXIT_FAILURE);
		}
	}
	if((outfile=fopen(outfilename,"w"))==NULL){
		fprintf(stderr,"can't open output file\n");
		exit(EXIT_FAILURE);
	}
#ifdef DEBUG
	printf("filesize: %llu bytes\n",input_filesize);
#endif
	ulonglong X_num_pixels;
	ulonglong Y_num_pixels;
	switch(output_mode){
		case MONO:
			X_num_pixels = (ulonglong)sqrt((X_ratio*input_filesize*BITWIDTH_CHAR)/Y_ratio)+1;
			X_num_pixels += (8-(X_num_pixels%8))%8;//pbmは幅を8の倍数にしなければならないので。
			Y_num_pixels = (ulonglong)sqrt((Y_ratio*input_filesize*BITWIDTH_CHAR)/X_ratio)+1;
			Y_num_pixels += (8-(Y_num_pixels%8))%8;//幅と揃えるため(特に正方形のとき)
			fprintf(outfile,"P4\n");
			fprintf(outfile,"%llu %llu\n",X_num_pixels,Y_num_pixels);
#ifdef DEBUG
	printf("mode: MONO\n");
#endif
			break;
		case GRAY:
			X_num_pixels = (ulonglong)sqrt((X_ratio*input_filesize)/Y_ratio)+1;
			Y_num_pixels = (ulonglong)sqrt((Y_ratio*input_filesize)/X_ratio)+1;
			fprintf(outfile,"P5\n");
			fprintf(outfile,"%llu %llu\n",X_num_pixels,Y_num_pixels);
			fprintf(outfile,"255\n");
#ifdef DEBUG
	printf("mode: GRAY\n");
#endif
			break;
		case COLOR:
			X_num_pixels = (ulonglong)sqrt((X_ratio*input_filesize/3.0)/Y_ratio)+1;
			Y_num_pixels = (ulonglong)sqrt((Y_ratio*input_filesize/3.0)/X_ratio)+1;
			fprintf(outfile,"P6\n");
			fprintf(outfile,"%llu %llu\n",X_num_pixels,Y_num_pixels);
			fprintf(outfile,"255\n");
#ifdef DEBUG
	printf("mode: COLOR\n");
#endif
			break;
	}
#ifdef DEBUG
	printf("X_num_pixels: %llu\n",X_num_pixels);
	printf("Y_num_pixels: %llu\n",Y_num_pixels);
	printf("X*Y: %llu\n",X_num_pixels*Y_num_pixels);
#endif
	ulonglong x,y;
	int pixel=0;
	for(y=0;y<Y_num_pixels;y++){
		for(x=0;x<X_num_pixels;x++){
			switch(output_mode){
				case MONO:
					if(x%8==0){
						if((pixel=fgetc(infile))==EOF){
							pixel=0;
						}
						fputc(pixel,outfile);
					}
					break;
				case GRAY:
					if((pixel=fgetc(infile))==EOF){
						pixel=0;
					}
					fputc(pixel,outfile);
					break;
				case COLOR:
					for(int i=0;i<3;i++){
						if((pixel=fgetc(infile))==EOF){
							pixel=0;
						}
						fputc(pixel,outfile);
					}
					break;
			}
		}
	}
	return 0;
}

char* add_extension(const char *filename,color_mode output_mode){
	char *extension;
	switch(output_mode){
		case MONO:
			extension=EXTENSION_MONO;
			break;
		case GRAY:
			extension=EXTENSION_GRAY;
			break;
		case COLOR:
			extension=EXTENSION_COLOR;
			break;
	}
	size_t length_outfilename = strlen(filename)+strlen(".")+strlen(extension)+1;//\0の分
	char *outfilename;
	if((outfilename=(char*)calloc(length_outfilename,sizeof(char)))==NULL){
		return NULL;
	}
	sprintf(outfilename,"%s.%s",filename,extension);
	return outfilename;
}
