#include <stdio.h>
#include <fcntl.h>
#include <string.h>

int main() {

char s_out[] = "out";
char s_gpio[] = "271";
char s_0[] = "0";
char s_1[] = "1";
int fd;

//export GPIO271

if((fd=open("/sys/class/gpio/export",O_WRONLY))<0) {
   printf("can't open export GPIO\r\n");
   return(1);
} else {
   if(write(fd,s_gpio,strlen(s_gpio))<0) {                                      
      printf("can't write to export GPIO\r\n");                                  
      return(1);                                                                
   }                                                                            
   printf("GPIO271 is exported\r\n");                                           
   if(close(fd)<0) {                                                            
      printf("can't close export GPIO\r\n");                                     
      return(1);                                                                
   }                                                                            
}                                                                               
                                                                                
//set as output GPIO271                                                         
                                                                                
if((fd=open("/sys/class/gpio/gpio271/direction",O_WRONLY))<0) {   
   printf("can't open GPIO direction\r\n");                                     
   return(1);                                                  
} else {                                                                          
   if(write(fd,s_out,strlen(s_out))<0) {                                         
      printf("can't write GPIO direction\r\n");                                     
      return(1);                        
   }                                        
   printf("GPIO271 is set as output\r\n");                                      
   if(close(fd)<0) {                                                             
      printf("can't close GPIO direction\r\n");                                     
      return(1);    
   }                                                            
}                                                                               
                                                                                
//toggle GPIO271                                                                
                                                                                
if((fd=open("/sys/class/gpio/gpio271/value",O_WRONLY))<0) {
   printf("can't open GPIO value\r\n"); 
   return(1);                                                                   
} else {                                                                          
                                                                                
   while(1) {                                                                   
                                                                                
      if(write(fd,s_1,strlen(s_1))<0) {                                           
         printf("can't write GPIO value\r\n"); 
         return(1);
      }                                                             
                                                                                
      if(write(fd,s_0,strlen(s_0))<0) {
         printf("can't write GPIO value\r\n"); 
         return(1);
      }                                                             
   };                                                                           
                                                                                
   if(close(fd)<0)                                                              
      return(1);                                                                
}                                                                               
                                                                                
}        
