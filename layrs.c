#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "ascii.h"
#include "functions.h"

void checkborder(double *pos, double *dir, double *dr ,  double *border){
    
    for(int k=0;k<3;k++){
        if(dir[k] > 0){
            border[k] = (floor(pos[k]/dr[k])+1)*dr[k];
        }
        else{
            if (floor(pos[k]/dr[k]) == pos[k]/dr[k] ){
                border[k] = (floor(pos[k]/dr[k])-1)*dr[k];
            }
            else{
                border[k] = (floor(pos[k]/dr[k]))*dr[k];
            }
        }
        
    }
    
}

double checklyr(double *pos, double *dir, double dz){
    
    double z_border=0;
    
    if(dir[2] > 0){
        z_border = (floor(pos[2]/dz)+1)*dz;
    }
    else{
        if (floor(pos[2]/dz) == pos[2]/dz ){
            z_border = (floor(pos[2]/dz)-1)*dz;
        }
        else{
            z_border = (floor(pos[2]/dz))*dz;
        }
    }
    return z_border;
}

double choosestep(double *pos, double *dir, double *border){
    
    double vstep[3];
    double tmp;
    for(int k=0;k<3;k++){
        
        vstep[k] = (border[k]-pos[k])/dir[k];
        
    }
    
    for(int k = 0;k<2;k++){
        
        if(vstep[k]<vstep[k+1]){
            tmp = vstep[k];
        }
        else{
            
            tmp = vstep[k+1];
        }
        
    }
    
    return tmp;
    
}



int main(){
    
    //     set box par
    
    time_t t;
    srand((unsigned) time(&t));
    
    double *z;
    double *k_s;
    double *k_a;
    int nlvl;
    
    read_3c_file ("./ksca_kabs/ksca_kabs_lambda350.dat", &z, &k_s, &k_a, &nlvl);
    
    
    int nlyr = nlvl-1;
//     double beta_a[nlyr];
    double beta_s[nlyr];
    double beta_ext[nlyr];
    double tau_lyr;
    double w0[nlyr];
    double tau_c = 10; 
    double beta_c[nlyr];
    double w0_lyr;

    double dz[nlyr];
    
    
    for(int i = 0; i<nlyr;i++){
        
        dz[i] = z[i]-z[i+1];
        beta_ext[i] = k_a[i+1] + k_s[i+1];
        
    }
    
    for(int i = 0; i<nlyr;i++){
//         beta_a[i] = k_a[i+1];
        beta_s[i] = k_s[i+1];
        
    }
    
    
    for(int j = 0;j<nlyr;j++){
     
        if(j>=5 && j<=15){
        
            beta_c[j] = beta_s[j] + 20;
        }
        else{
            beta_c[j] = beta_s[j];
        }
        w0[j] = beta_c[j]/(beta_c[j]+k_a[j+1]);
    }
    
    int dim = 3;
    
    int N_sca_up=0;
    int N_abs=0;
    int N_sca_temp=0;
    int N_dn=0;
    int N_up=0;
    int N_sca_avg=0;
    int Ntot=1000000;
    
    double pos[]    = {1,10,120};
    double pos_f[dim];
//     double dir_f[dim];
    double dir[dim];
//     double border;
    
        
    
    
    
    //     printf("%f  %f  %f\n",pos_f[0],pos_f[1],pos_f[2]);
    
    
    //     photon loop
    int I=0;
    int N_sca_m=0;
    
    
    
    while(I<Ntot){
        
        
        
        double tau = taufmp();
        int j =0;
        double path [3];
        
        int N_sca=0;
        
        pos[0]=1;
        pos[1]=10;
        pos[2]=120;
        
        double theta_s  = 30*M_PI/180.;
        double phi_s    = 0*M_PI/180.;
        
        dir[0]=sin(theta_s)*cos(phi_s);
        dir[1]=sin(theta_s)*sin(phi_s);
        dir[2]=-cos(theta_s);
        
        
        while(1==1){
                        
            double z_border = checklyr(pos,dir,dz[j]);
            double step = fabs((pos[2]-z_border)/dir[2]);
            
            for(int k=0;k<3;k++){
                path[k]=dir[k]*step;
                pos_f[k] = pos[k] + path[k];
            }
            double ds = givelen(path,3);
            
            
            
            tau_lyr = beta_ext[j]*ds;
            if(tau_lyr>= tau){
                double tmp_vec[3];
                double b = randnum();
                double r = randnum();
                
                if(b<w0[j]){
                    if(r>(beta_c[j]-beta_s[j])/beta_c[j]){    
                    step = tau/tau_lyr*step;
                    for(int k=0;k<3;k++){
                        path[k]=dir[k]*step;
                        pos[k] = pos[k] + path[k];
                    }
                    
                    N_sca++;        
                    N_sca_temp++;
                    
                    scattering_ray(pos,dir,tmp_vec);
                    }
                    else{
                    step = tau/tau_lyr*step;
                    for(int k=0;k<3;k++){
                        path[k]=dir[k]*step;
                        pos[k] = pos[k] + path[k];
                    }
                    
                    N_sca++;        
                    N_sca_temp++;
                    
                    scattering(pos,dir,tmp_vec);
                    }
                    tau = taufmp();
                     //     box def properties
    
                    for(int k=0;k<3;k++){
                        dir[k] = tmp_vec[k];
                    }
                    
                }
                else{
                    N_abs++;
                    break;
                }
                
                
            }
            
            else{
                tau=tau-tau_lyr;
                if(pos_f[2]>=z[0]-0.1){
                    N_up++;
                    
                    break;
                }
                else if(pos_f[2]<=0.0001){
                    double a =randnum();
                    double A=0.13;
                    if(a<A){
                        
                        double theta_sfc = sfcrefl();
                        double phi_sfc = randphi();
                        
                        dir[0]=sin(theta_sfc)*cos(phi_sfc);
                        dir[1]=sin(theta_sfc)*sin(phi_sfc);
                        dir[2]=cos(theta_sfc);
                    }
                    else{
                        N_dn++;
                    
                    
                    break;
                    }
                }
                
                for(int k =0;k<3;k++){
                    pos[k]=pos_f[k];
                }
                
                //         lyr update
                
                if (dir[2]<0){
                    j++;
                }
                else{
                    j--;
                }
            }            
            if(N_sca>N_sca_m){
                N_sca_m = N_sca;   
            }
            
        }
        I++;
        
    }
    N_sca_avg = N_sca_temp/Ntot;
    printf("N_dn = %d   N_abs = %d N_up = %d  I = %d  N_sca_avg = %d    N_sca_max = %d\n", N_dn, N_abs, N_up, I, N_sca_avg,N_sca_m);
}