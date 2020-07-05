#include<stdio.h>
#include<stdlib.h>
#include<strings.h>
#include<math.h>

#define PGM_MAGIC_NUM 5

#ifndef TRUE
 #define TRUE 1
#endif

#ifndef FALSE
 #define FALSE 0
#endif


typedef struct  {
        int width ;
        int height ;
        int maxgrey ;
        } pgm_header ;

typedef struct  {
        double     xmax ;
        double     xmin ;
        double     ymax ;
        double     ymin ;
        double     limit ;
        int        max_iter; 
        char    do_grads;
        int     oversample; 
        } fractal ;

typedef struct  {
        double real ;
        double imag ;
        } imag_num ;




int write_pgm_head( pgm_header *, FILE * );
void assign_imag( imag_num *, double ,double );
void add_imag( imag_num *, imag_num * );
void mult_imag( imag_num *, imag_num * );


int main( int argc, char **argv )
{


    pgm_header  pgm_head ;
    imag_num    z, c ;
    fractal     frac ;

    char       *data ;
    int        i, j, k, ii, jj, over_sqrd; 
    double     x, y, iover, jover, over_step_size;
    double     dist, out ;
    FILE       *out_stream=stdout ;
    
    void fracpgm_clparse( int, char **, pgm_header *, fractal * ) ;
    
    /*defaults*/
    pgm_head.width=500;
    pgm_head.height=500;
    pgm_head.maxgrey=255;

    frac.xmin = -2   ;
    frac.xmax =  1   ;
    frac.ymin = -1.5 ;
    frac.ymax =  1.5 ;

    frac.limit = 1 + sqrt( 2.0 );
    frac.max_iter = 500 ;
    frac.oversample = 1 ;

    frac.do_grads = FALSE ;

    fracpgm_clparse( argc, argv, &pgm_head, &frac ) ;

    data = ( char *)malloc( sizeof( char ) * pgm_head.width * pgm_head.height );
    over_sqrd=frac.oversample * frac.oversample;
    over_step_size=(1.0/(float)(frac.oversample+1.0));


    for( i=0; i<pgm_head.height; i++ ){
        for( j=0; j<pgm_head.width; j++ ){
            
            *( data + i * pgm_head.width + j ) = 0 ;
            out=0.;
            
            for( ii=0; ii<frac.oversample; ii++ ){

                iover = (ii+1)*over_step_size;
                y = frac.ymax - ( i + iover ) * ( frac.ymax - frac.ymin ) / ( pgm_head.height) ;

                for( jj=0; jj<frac.oversample; jj++ ){
                    
                    jover = (jj+1)*over_step_size;
                    x = frac.xmin + ( j + jover ) * ( frac.xmax - frac.xmin ) / ( pgm_head.width );
 
                    assign_imag( &c, x, y );
                    assign_imag( &z, 0, 0 );
                    
                    for( k=0; k<frac.max_iter; k++ ){
                    
                        if( z.real == 0 && z.imag == 0 ) dist = 0 ;
                        else dist = sqrt( pow( ( z.real - z.imag ), 2 ) ) ;
                        
                        if( dist > frac.limit ){
                            if( frac.do_grads == TRUE )
                                out +=  k / (float)frac.max_iter ;
                            else
                                out += 1 ;
                            break ;
                        }
                        
                        mult_imag( &z, &z );
                        add_imag ( &z, &c );
                        
                    }
                }
            }
            
            *( data + i * pgm_head.width + j ) = (char)(pgm_head.maxgrey*out/(float)over_sqrd) ;
        }
    }

    write_pgm_head( &pgm_head, out_stream ) ;
    fwrite( data, sizeof( char ), pgm_head.width * pgm_head.height, out_stream ) ;

    return( EXIT_SUCCESS );
    
}


void mult_imag( z1, z2 )
imag_num *z1, *z2 ;
{

    imag_num z;
    
    z.real = z1->real * z2->real - z1->imag * z2->imag ;
    z.imag = z1->real * z2->imag + z1->imag * z2->real ;

    z1->real = z.real ;
    z1->imag = z.imag ;

    return;
}

void add_imag( z1, z2 )
imag_num *z1, *z2 ;
{

    
    z1->real += z2->real ;
    z1->imag += z2->imag ;

    return;
}


void assign_imag( z, r ,i )
imag_num * z ;
double r, i;
{

    z->real=r;
    z->imag=i;

    return;
}


int write_pgm_head( pgm_head, stream )
pgm_header     *pgm_head ;
FILE         *stream ;
{

    fprintf( stream, "P%d\n", PGM_MAGIC_NUM   );
    fprintf( stream, "%d\n", pgm_head->width   );
    fprintf( stream, "%d\n", pgm_head->height  );
    fprintf( stream, "%d\n", pgm_head->maxgrey );


    return( 1 );
}



void fracpgm_clparse( argc, argv, pgm_head, frac )
int           argc;
char        **argv;
pgm_header      *pgm_head ;
fractal      *frac ;
{


    int     i, use_aspect=1 ;
    double  aspect_ratio=1., zoom=1., xpos=-0.5, ypos=0.;
    void    usage();
        
    for( i = 1; i < argc; i++ ){
        
        if( *argv[ i ] == '-' ){    
    
                 if( !strncasecmp( argv[ i ], "-width", 2  ) ) pgm_head->width  = atoi( argv[ ++i ] ) ;
            else if( !strncasecmp( argv[ i ], "-aspect", 2 ) ){
                 aspect_ratio = atof( argv[ ++i ] ) ;
                 use_aspect = 1;
            }     
            else if( !strncasecmp( argv[ i ], "-height", 2 ) ){
                pgm_head->height= atof( argv[ ++i ] ) ;
                use_aspect = 0 ;
            }
            else if( !strncasecmp( argv[ i ], "-xpos", 5 ) ) xpos = atof( argv[ ++i ] ) ;
            else if( !strncasecmp( argv[ i ], "-ypos", 5 ) ) ypos = atof( argv[ ++i ] ) ;
            else if( !strncasecmp( argv[ i ], "-zoom", 5 ) ) zoom = atof( argv[ ++i ] ) ;
            
            else if( !strncasecmp( argv[ i ], "-over", 2 ) ) frac->oversample = atoi( argv[ ++i ] ) ;
            else if( !strncasecmp( argv[ i ], "-iter",  5 ) ) frac->max_iter = atof( argv[ ++i ] ) ;
            else if( !strncasecmp( argv[ i ], "-limit", 5 ) ) frac->limit    = atoi( argv[ ++i ] ) ;            
            else if( !strncasecmp( argv[ i ], "-grads", 2 ) ) frac->do_grads = TRUE  ;
            else if( !strncasecmp( argv[ i ], "-u", 2 ) ) usage( )  ;
            
            
            else{
        
                fprintf( stderr, "%s: unknown argument on command line: %s\n", argv[ 0 ], argv[ i ] );
                fprintf( stderr, "(use the option -u to see brief usage instructions)\n" );
                exit( EXIT_FAILURE );
        
            }
        }
        
    }
    
    /*
    translate options into the parameters needed
    by the code
    */
    if( use_aspect==1 ){
        pgm_head->height = pgm_head->width / aspect_ratio ;
    }else{
        aspect_ratio= pgm_head->width/(float)pgm_head->height ;
    }
    
    frac->xmin = xpos-1.5/zoom;
    frac->xmax = xpos+1.5/zoom;
    frac->ymin = ypos-1.5/aspect_ratio/zoom;
    frac->ymax = ypos+1.5/aspect_ratio/zoom;
    
    
    
    return ;
    
}


void usage( )
{
    fprintf(stderr, "usage: fracpgm3 [options] > output.ppm\n\n");
    fprintf(stderr, "where [options] are:\n");
    fprintf(stderr, "-width n    where n (int) is the width of the image in pixels\n");
    fprintf(stderr, "-aspect n   where n (float) is the aspect ratio of the image (width/height)\n");
    fprintf(stderr, "-xpos n     where n (float) is the x coordinate in the fractal of the middle pixel in the image\n");
    fprintf(stderr, "-ypos n     where n (float) is the x coordinate in the fractal of the middle pixel in the image\n");
    fprintf(stderr, "-over n     where n (int) is the number of times to oversample each pixel (in each axis)\n");
    fprintf(stderr, "-iter n     where n (int) is the maximum number of itterations to use for each pixel\n");
    exit( EXIT_SUCCESS );

}


