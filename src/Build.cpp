/*
 * Build.cpp
 * Copyright (c) 2020 Yasuo Tabei All Rights Reserved.    
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include "gWM.hpp"

using namespace std;

char *fname, *indexFile;
int  iteration = 2;

void usage();
void version();
void parse_parameters (int argc, char **argv);

int main(int argc, char **argv) {
  parse_parameters(argc, argv);

  gWM gwm;
  gwm.constructor(fname, indexFile, iteration);
}

void version(){
  std::cerr << "gWM version 3.1.1" << std::endl
            << "Written by Yasuo Tabei" << std::endl << std::endl;
}

void usage(){
  std::cerr << std::endl
       << "Usage: gwm-build [OPTION]... DATABASEFILE INDEXFILE" << std::endl << std::endl
       << "       where [OPTION]...  is a list of zero or more optional arguments" << std::endl
       << "             DATABASEFILE  is the name of a graph database" << std::endl
       << "             INDEXFILE     is the name of an indexfile" << std::endl << std::endl
       << "Additional arguments (input and output files may be specified):" << std::endl
       << "       -iteration [# of iteration]: number of WLKernel computation" << std::endl
       << "       (default: " << iteration << ")" << std::endl
       << std::endl;
  exit(0);
}

void parse_parameters (int argc, char **argv){
  if (argc == 1) usage();
  int argno;
  for (argno = 1; argno < argc; argno++){
    if (argv[argno][0] == '-'){
      if      (!strcmp (argv[argno], "-version")){
	version();
      }
      else if (!strcmp (argv[argno], "-iteration")) {
	if (argno == argc - 1) std::cerr << "Must specify # of iteration after -iteration" << std::endl;
	iteration = atoi(argv[++argno]);
      }
      else {
	usage();
      }
    } else {
      break;
    }
  }
  if (argno + 1 >= argc)
    usage();

  fname     = argv[argno];
  indexFile = argv[argno + 1];
}
