#include <string.h>
#include <iostream.h>
#include <unistd.h>

using namespace std;

int main()
{
	// get youtube top 20
	system("wget http://www.youtube.com/charts");
	// parse for links
	system("cat charts | grep data-video-ids= > out.txt");

	FILE *fp  = fopen("out.txt", "r");
	char buf[1024];
	int i = 1;
	while(fscanf(fp, "<a href=%s \n", buf) != EOF)
	{
		char buf2[1024];
		printf("read: %s", buf);
		
		// download video
		sprintf(buf2, "python youtube.py www.youtube.com%s -f 18 -o video/%d.mp4", buf, i);
		system(buf2);
		fgets(buf2, 10000, fp);
		
		// strip audio
		sprintf(buf2, "ffmpeg -i video/%d.mp4 -vn -acodec pcm_s16le -ar 44100 -ac 2 audio/%d.wav", i, i);
		system(buf2);
		
		// strip video
		
		i++;
	}
	fclose(fp);
}

