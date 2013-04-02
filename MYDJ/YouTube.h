// library: YouTube Download Library
//
// description:
//	library for generating YouTube download links
//	library for downloading YouTube videos to hard drive or ram
//
// dependencies:
//	openssl library: https://www.openssl.org/

namespace YouTube {

	// function: get youtube download link
	// 
	// description:
	//	obtains video data and generate custom download link based on the host computers settings.
	//
	// params:
	//	videoID: the id of the target youtube video, eg: https://www.youtube.com/watch?v=D0ZUDvoJFzY the id will be: D0ZUDvoJFzY
	//	option1: 1st most preferred itag
	//	option2: 2nd most preferred itag
	//	option3: 3nd most preferred itag
	//
	// notes:
	//	if none of the user choosen itag is found, function will return the download link for the first itag it parses.
	//	itag codes can be found here: https://en.wikipedia.org/wiki/YouTube#Quality_and_codecs
	std::string GetDownloadLink (const std::string & videoID, std::string option1, std::string option2, std::string option3);
	
	// function: download video to hdd
	//
	// description:
	//	downloads target video to program hard drive. this download mode is 
	//	recommended to save on ram.
	// 
	// params:
	//	address:	youtube download address for target video (obtained from GetDownloadLink)
	//	virtualhdd: storage for downloaded video
	//
	// Notes:
	//	to reclaim used virtual hard drive space: clear and shrink variable
	//	virtualhdd.clear();	virtualhdd.shrink_to_fit();
	int DownloadVideoToHDD (const std::string & address, const std::string & path);

	// function: download video to ram
	//
	// description:
	//	downloads target video to program ram. this download mode is 
	//	recommended to save on write cycles to harddrive.
	// 
	// Params:
	//	address:	youtube download address for target video (obtained from GetDownloadLink)
	//	virtualhdd: storage for downloaded video
	//
	// Notes:
	//	to reclaim used virtual hard drive space: clear and shrink variable
	//	virtualhdd.clear();	virtualhdd.shrink_to_fit();
	int DownloadVideoToRAM (const std::string & address, std::string & virtualhdd);
}