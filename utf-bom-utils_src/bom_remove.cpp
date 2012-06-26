/*


*/

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>

#include "CDirectoryExplorer.h"

using namespace std;
using namespace mrpt::system;


// ------------------------------------------------------
//                        MAIN
// ------------------------------------------------------
int main(int argc, char** argv)
{
	try
	{
		if (argc<2)
		{
			printf("Usage: %s source1.cpp [source2.cpp ...]\n",argv[0]);
			return -1;
		}

		for (int i=1;i<argc;i++)
		{
			CDirectoryExplorer::TFileInfoList lst;
			CDirectoryExplorer::explore(argv[i],FILE_ATTRIB_ARCHIVE,lst);

			if (lst.empty())
			{
				cerr << "Error: Not found " << argv[i] << endl;
				continue;
			}

			for (CDirectoryExplorer::TFileInfoList::const_iterator it=lst.begin();it!=lst.end();it++)
			{
				FILE *f = fopen(it->wholePath.c_str(),"rb");
				if (!f)
				{
					cerr << "Error: Cannot open" << it->wholePath << endl;
					continue;
				}
				unsigned char buf[10];
				fread(buf,1,3,f);

				// Get file size:
				fseek(f,0,SEEK_END);
				const long N = ftell(f);
				fclose(f);

				if (!(buf[0]==0xEF && buf[1]==0xBB && buf[2]==0xBF))
				{
					cout  << "Skipping (has no BOM): " << it->wholePath << endl;
					continue;
				}

				// Remove BOM: =======================================
				// Read the whole file:
				unsigned char *fil_buf = new unsigned char[N];

				f = fopen(it->wholePath.c_str(),"rb");
				fread(fil_buf,1,N,f);
				fclose(f);

				// Write new version:
				f = fopen(it->wholePath.c_str(),"wb");
				fwrite(fil_buf+3,1,N-3,f);
				fclose(f);

				delete[] fil_buf;

				cout << "Removed BOM: " << it->wholePath << endl;
			}
		}
		
		return 0;
	} catch (exception &e)
	{
		cerr << "EXCEPCTION: " << e.what() << endl;
		return -1;
	}
	catch (...)
	{
		cerr << "Untyped excepcion!!";
		return -1;
	}
}


