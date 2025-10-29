#define _CRT_SECURE_NO_WARNINGS
#include "MyBot.h"
#include "token.h"
#include "Bot_Debug.h"
#include "Audio.h"
#include "zip.h"
#include <string>
#include <vector>
#include <dpp/dpp.h>

const std::string    BOT_TOKEN = TOKEN;

extern zip_t* zipArchive;
extern Audio* audio;
extern bool audio_present;
extern int audio_sample_rate;
extern int audio_channels;
extern uint64_t audio_samples;
extern int16_t* audio_buffer;

extern void ParseAnimation();
extern void ExtractDescTxt();
extern void ProcessZipFile();
extern void SortZipEntries();
extern void SetDynamicColors(std::string R, std::string G, std::string B, std::string A);

std::string dynR;
std::string dynG;
std::string dynB;
std::string dynA;

bool dyn_coloring_enabled = false;

//https://stackoverflow.com/a/46931770
std::vector<std::string> split(const std::string& s, char delim) {
	std::vector<std::string> result;
	std::stringstream ss(s);
	std::string item;

	while (getline(ss, item, delim)) {
		result.push_back(item);
	}

	return result;
}

int main()
{
	/* Create bot cluster */
	dpp::cluster bot(BOT_TOKEN);

	/* Output simple log messages to stdout */
	bot.on_log(dpp::utility::cout_logger());

	/* Register slash command here in on_ready */
	bot.on_ready([&bot](const dpp::ready_t& event) {
		if (dpp::run_once<struct register_bot_commands>()) {
			dpp::slashcommand command("tomp4", "Convert to MP4", bot.me.id);
			// Add file and name as required parameters
			command.add_option(dpp::command_option(dpp::co_attachment, "anim", "Bootanimation file", true));
			command.add_option(dpp::command_option(dpp::co_attachment, "audio", "Sound played with bootanimation", false));
			command.add_option(dpp::command_option(dpp::co_string, "dyncolor", "Dynamic color (format: intR,intG,intB,intA)", false));
			bot.global_command_create(command);
		}
	});

	/* Handle slash command with the most recent addition to D++ features, coroutines! */
	bot.on_slashcommand([](const dpp::slashcommand_t& event) -> dpp::task<void> {
		if (event.command.get_command_name() == "tomp4") {
			dpp::cluster* cluster = event.owner;

			dpp::async thinking = event.co_thinking(false);

			if (event.get_parameter("audio").index() != 0)
				audio_present = true;

			dpp::snowflake anim_fileid = std::get<dpp::snowflake>(event.get_parameter("anim"));
			BotLogger(LOGLEVEL_DEBUG, "-----> anim fileid: %llu", anim_fileid);

			const dpp::attachment& anim_attachment = event.command.get_resolved_attachment(anim_fileid);
			dpp::http_request_completion_t anim_response = co_await cluster->co_request(anim_attachment.url, dpp::m_get);

			if (anim_response.status != 200) { // Page didn't send the image
				co_await thinking; // Wait for the thinking response to arrive so we can edit
				BotLogger(LOGLEVEL_DEBUG, "-----> failed downloading anim (%d)", anim_response.status);
				event.edit_response("Error: serverside could not download the attachment (animation)");
			}

			//save it somewhere (in that case temp)
			std::ofstream anim_file("~temp_anim", std::ios::binary);
			if (!anim_file.is_open()) {
				co_await thinking; // Wait for the thinking response to arrive so we can edit
				BotLogger(LOGLEVEL_DEBUG, "-----> failed open tempfile");
				event.edit_response("Error: serverside could not open file for writing (animation)");
				co_return;
			}
			anim_file.write(anim_response.body.data(), anim_response.body.size());
			anim_file.close();

			if (audio_present == true)
			{
				dpp::snowflake audio_fileid = std::get<dpp::snowflake>(event.get_parameter("audio"));
				BotLogger(LOGLEVEL_DEBUG, "-----> audio fileid: %llu", audio_fileid);
				const dpp::attachment& audio_attachment = event.command.get_resolved_attachment(audio_fileid);
				dpp::http_request_completion_t audio_response = co_await cluster->co_request(audio_attachment.url, dpp::m_get);
				if (audio_response.status != 200) { // Page didn't send the image
					co_await thinking; // Wait for the thinking response to arrive so we can edit
					BotLogger(LOGLEVEL_DEBUG, "-----> failed downloading audio (%d)", anim_response.status);
					event.edit_response("Error: serverside could not download the attachment (audio)");
				}

				//save it somewhere (in that case temp)
				std::ofstream audio_file("~temp_audio", std::ios::binary);
				if (!audio_file.is_open()) {
					co_await thinking; // Wait for the thinking response to arrive so we can edit
					BotLogger(LOGLEVEL_DEBUG, "-----> failed open tempfile");
					event.edit_response("Error: serverside could not open file for writing (audio)");
					co_return;
				}
				audio_file.write(audio_response.body.data(), audio_response.body.size());\
				audio_file.close();
				audio = new Audio();
			}

			co_await thinking; // Wait for the thinking response to arrive so we can edit
			BotLogger(LOGLEVEL_DEBUG, "-----> done downloading file(s)");

			zipArchive = zip_open("~temp_anim", ZIP_RDONLY, nullptr);

			if (!zipArchive) {
				BotLogger(LOGLEVEL_DEBUG, "-----> failed opening zip archive");
				event.edit_response("Error: serverside could not open the bootanimation file (zip)");
				co_return;
			}

			if (audio_present)
			{
				audio->Open(std::string("~temp_audio"));
				audio->Convert();
				audio->ExtractInfo(&audio_sample_rate, &audio_samples, &audio_channels);
				audio_buffer = audio->GetAudioBuffer();
			}
			ProcessZipFile();
			SortZipEntries();

			if (event.get_parameter("dyncolor").index() != 0)
			{
				//formatted as (intR),(intG),(intB),(intA)
				std::string dyncolor = std::get<std::string>(event.get_parameter("dyncolor"));
				BotLogger(LOGLEVEL_DEBUG, "-----> dyncolor: %s", dyncolor.c_str());
				std::vector<std::string> colors = split(dyncolor, ',');
				if (colors.size() != 4)
				{
					BotLogger(LOGLEVEL_DEBUG, "-----> dyncolor: invalid format");
					event.edit_response("Error: invalid dynamic color format (must be 4 integers separated by ','");
					co_return;
				}
				dynR = colors[0];
				dynG = colors[1];
				dynB = colors[2];
				dynA = colors[3];
			}
			ExtractDescTxt();
			SetDynamicColors(dynR, dynG, dynB, dynA);
			ParseAnimation();
			zip_close(zipArchive);

			if (audio_present)
			{
				audio->Close();
			}

			//send file
			dpp::message msg(event.command.channel_id, "");

			//get time
			std::time_t now = std::time(nullptr);
			std::tm* local_time = std::localtime(&now);
			char time_str[100];
			std::strftime(time_str, sizeof(time_str), "%Y-%m-%d_%H-%M-%S", local_time);
			//set filename
			std::string filename = "bootanimation_" + std::string(time_str) + ".mp4";

			msg.add_file(filename, dpp::utility::read_file("~temp_output"));

			/* Send the message, with our attachment. */
			co_await thinking;
			event.edit_original_response(msg);

			//delete temp files
			std::remove("~temp_anim");
			std::remove("~temp_audio");
			std::remove("~temp_output");
		}
		co_return;
	});

	/* Start the bot */
	bot.start(dpp::st_wait);

	return 0;
}
