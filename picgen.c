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

typedef enum{
	X_RATIO,
	Y_RATIO,
	X_ABSOLUTE,
	Y_ABSOLUTE,
	MODE,
	HELP,
	NO_EXTENSION
} type_argument;
char* add_extension(const char*,color_mode);
void add_header(FILE* ,color_mode ,ulonglong ,ulonglong );
int main(int argc,char *argv[]){
	FILE *infile = NULL,*outfile = NULL;
	double X_ratio = 1.0;
	double Y_ratio = 1.0;
	ulonglong X_num_pixels;
	ulonglong Y_num_pixels;
	bool X_absolutely = false;
	bool Y_absolutely = false;
	color_mode output_mode = COLOR;
	bool help_required = false;
	bool no_extension = false;
	char *extension;
	struct option longopts[]={
		{"X_ratio"     ,required_argument,0,X_RATIO     },
		{"Y_ratio"     ,required_argument,0,Y_RATIO     },
		{"X_absolute"  ,required_argument,0,X_ABSOLUTE  },
		{"Y_absolute"  ,required_argument,0,Y_ABSOLUTE  },
		{"mode"        ,required_argument,0,MODE        },
		{"help"        ,no_argument      ,0,HELP        },//flagにはintが入れられて、_Boolとはなじまないので
		{"no_extension",no_argument      ,0,NO_EXTENSION},//flagにはintが入れられて、_Boolとはなじまないので
		{0             ,0                ,0,0           },
	};
	int idx_lngopt=0;
	int val_opt=0;
	while((val_opt=getopt_long(argc,argv,"X:Y:x:y:m:h",longopts,&idx_lngopt))!=-1){
		char *err=NULL;
		switch(val_opt){
			case X_RATIO:
			case 'X':;
				double X_ratio_tmp=(double)strtod(optarg,&err);
				if(*err != '\n' && *err != '\0'){
					fprintf(stderr,"error: invalid value %s\n",optarg);
				}else{
					X_ratio=X_ratio_tmp;
				}
				break;
			case Y_RATIO:
			case 'Y':;
				double Y_ratio_tmp=(double)strtod(optarg,&err);
				if(*err != '\n' && *err != '\0'){
					fprintf(stderr,"error: invalid value %s\n",optarg);
				}else{
					Y_ratio=Y_ratio_tmp;
				}
				break;
			case X_ABSOLUTE:
			case 'x':;
				ulonglong X_num_pixels_tmp=(ulonglong)strtol(optarg,&err,0);
				if(*err != '\n' && *err != '\0'){
					fprintf(stderr,"error: invalid value %s\n",optarg);
				}else{
					X_num_pixels=X_num_pixels_tmp;
					X_absolutely=true;
				}
				break;
			case Y_ABSOLUTE:
			case 'y':;
				ulonglong Y_num_pixels_tmp=(ulonglong)strtol(optarg,&err,0);
				if(*err != '\n' && *err != '\0'){
					fprintf(stderr,"error: invalid value %s\n",optarg);
				}else{
					Y_num_pixels=Y_num_pixels_tmp;
					Y_absolutely=true;
				}
				break;
			case MODE:
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
			case HELP:
			case 'h':
				help_required = true;
				break;
			case NO_EXTENSION:
			case 'n':
				no_extension = true;
				break;
			case '?':
				//printf("error\n");
				exit(EXIT_FAILURE);
				break;
		}
	}
	if(help_required){
		printf(APPNAME" [options] input_filepath output_filepath                 \n");
		printf(APPSPAC" options                                                  \n");
		printf(APPSPAC" -h, --help                    :show this help and exit   \n");
		printf(APPSPAC" -X, --X_ratio NUM             :x ratio of output image   \n");
		printf(APPSPAC" -Y, --Y_ratio NUM             :y ratio of output image   \n");
		printf(APPSPAC" -x, --X_absolute NUM          :absolute x size (pixel)   \n");
		printf(APPSPAC" -y, --Y_absolute NUM          :absolute y size (pixel)   \n");
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
	ulonglong num_all_pixels;
	switch(output_mode){
		case MONO:
			num_all_pixels=(ulonglong)(input_filesize*BITWIDTH_CHAR);
#ifdef DEBUG
			printf("mode: MONO\n");
#endif
			break;
		case GRAY:
			num_all_pixels=(ulonglong)(input_filesize);
#ifdef DEBUG
			printf("mode: GRAY\n");
#endif
			break;
		case COLOR:
			num_all_pixels=(ulonglong)(input_filesize/3.0);
#ifdef DEBUG
			printf("mode: COLOR\n");
#endif
			break;
	}
#ifdef DEBUG
	printf("num_all_pixels: %llu\n",num_all_pixels);
#endif
	if((X_absolutely || Y_absolutely) && !(X_absolutely && Y_absolutely)){
		if(X_absolutely){
			Y_num_pixels=(ulonglong)ceil((num_all_pixels/(double)X_num_pixels));
		}else{
			X_num_pixels=(ulonglong)ceil((num_all_pixels/(double)Y_num_pixels));
		}
		if(output_mode==MONO){
			X_num_pixels += (8-(X_num_pixels%8))%8;//pbmは幅を8の倍数にしなければならないので。
			Y_num_pixels += (8-(Y_num_pixels%8))%8;//幅と揃えるため(特に正方形のとき)
		}
	}else if(!(X_absolutely || Y_absolutely)){
		X_num_pixels = (ulonglong)ceil(sqrt((X_ratio*num_all_pixels)/Y_ratio));
		Y_num_pixels = (ulonglong)ceil(sqrt((Y_ratio*num_all_pixels)/X_ratio));
		if(output_mode==MONO){
			X_num_pixels += (8-(X_num_pixels%8))%8;//pbmは幅を8の倍数にしなければならないので。
			Y_num_pixels += (8-(Y_num_pixels%8))%8;//幅と揃えるため(特に正方形のとき)
		}
	}
	add_header(outfile,output_mode,X_num_pixels,Y_num_pixels);
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

void add_header(FILE* outfile,color_mode output_mode,ulonglong X_size,ulonglong Y_size){
	switch(output_mode){
		case MONO:
			fprintf(outfile,"P4\n");
			fprintf(outfile,"%lld %lld\n",X_size,Y_size);
			break;
		case GRAY:
			fprintf(outfile,"P5\n");
			fprintf(outfile,"%lld %lld\n",X_size,Y_size);
			fprintf(outfile,"255\n");
			break;
		case COLOR:
			fprintf(outfile,"P6\n");
			fprintf(outfile,"%lld %lld\n",X_size,Y_size);
			fprintf(outfile,"255\n");
			break;
	}
}
