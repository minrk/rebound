/**
 * @file 	gravity.c
 * @brief 	Direct gravity calculation, O(N^2).
 * @author 	Hanno Rein <hanno@hanno-rein.de>
 *
 * @details 	This is the crudest implementation of an N-body code
 * which sums up every pair of particles. It is only useful very small 
 * particle numbers (N<~100) as it scales as O(N^2). 
 *
 * 
 * @section LICENSE
 * Copyright (c) 2011 Hanno Rein, Shangfei Liu
 *
 * This file is part of rebound.
 *
 * rebound is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * rebound is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with rebound.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include "particle.h"
#include "main.h"
#include "boundaries.h"

#ifdef MPI
#error GRAVITY_DIRECT not compatible with MPI
#endif

void gravity_calculate_acceleration(){
#pragma omp parallel for
	for (int i=0; i<N; i++){
		particles[i].ax = 0; 
		particles[i].ay = 0; 
		particles[i].az = 0; 
	}
	// Summing over all Ghost Boxes
	int _N_active_last = (N_active_last==-1)?N:N_active_last;
	for (int gbx=-nghostx; gbx<=nghostx; gbx++){
	for (int gby=-nghosty; gby<=nghosty; gby++){
	for (int gbz=-nghostz; gbz<=nghostz; gbz++){
		struct ghostbox gb = boundaries_get_ghostbox(gbx,gby,gbz);
		// Summing over all particle pairs
#pragma omp parallel for
		for (int i=0; i<N; i++){
		for (int j=N_active_first; j<_N_active_last; j++){
			if (i==j) continue;
			double dx = (gb.shiftx+particles[i].x) - particles[j].x;
			double dy = (gb.shifty+particles[i].y) - particles[j].y;
			double dz = (gb.shiftz+particles[i].z) - particles[j].z;
			double r = sqrt(dx*dx + dy*dy + dz*dz + softening*softening);
			double prefact = -G/(r*r*r)*particles[j].m;
			particles[i].ax += prefact*dx; 
			particles[i].ay += prefact*dy; 
			particles[i].az += prefact*dz; 
		}
		}
	}
	}
	}
}
