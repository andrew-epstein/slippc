#include "replay.h"

//Indent Level
#define ILEV 1

//Output shortcuts
#define JFLT(i,k,n) SPACE[ILEV*(i)] << "\"" << (k) << "\" : " << float(n)
#define JINT(i,k,n) SPACE[ILEV*(i)] << "\"" << (k) << "\" : " << int32_t(n)
#define JUIN(i,k,n) SPACE[ILEV*(i)] << "\"" << (k) << "\" : " << uint32_t(n)
#define JSTR(i,k,s) SPACE[ILEV*(i)] << "\"" << (k) << "\" : \"" << (s) << "\""
#define CHANGED(field) (not delta) || (f == 0) || (s.player[p].frame[f].field != s.player[p].frame[f-1].field)
#define JEND(a) ((a++ == 0) ? "\n" : ",\n")

//Strings for various indentation amounts
const std::string SPACE[10] = {
  "",
  " ",
  "  ",
  "   ",
  "    ",
  "     ",
  "      ",
  "       ",
  "        ",
  "         ",
};

namespace slip {

void setFrames(SlippiReplay &s, int32_t nframes) {
  s.frame_count = nframes;
  for(unsigned i = 0; i < 4; ++i) {
    if (s.player[i].player_type != 3) {
      s.player[i].frame = new SlippiFrame[nframes+123];
      if (s.player[i].ext_char_id == 0x0E) { //Ice climbers
        s.player[i+4].frame = new SlippiFrame[nframes+123];
      }
    }
  }
}

void cleanup(SlippiReplay s) {
  for(unsigned i = 0; i < 4; ++i) {
    delete s.player[i].frame;
  }
}

std::string replayAsJson(SlippiReplay s, bool delta) {
  int32_t total_frames = s.frame_count + 123 + 1;
  std::stringstream ss;
  ss << "{" << std::endl;

  ss << JSTR(0,"slippi_version", s.slippi_version) << ",\n";
  ss << JSTR(0,"parser_version", "0.0.1")          << ",\n";
  ss << JSTR(0,"game_start_raw", s.game_start_raw) << ",\n";
  ss << JUIN(0,"teams"         , s.teams)          << ",\n";
  ss << JUIN(0,"stage"         , s.stage)          << ",\n";
  ss << JUIN(0,"seed"          , s.seed)           << ",\n";
  ss << JUIN(0,"pal"           , s.pal)            << ",\n";
  ss << JUIN(0,"frozen"        , s.frozen)         << ",\n";
  ss << JUIN(0,"end_type"      , s.end_type)       << ",\n";
  ss << JINT(0,"lras"          , s.lras)           << ",\n";
  ss << JINT(0,"first_frame"   , -123)             << ",\n";
  ss << JINT(0,"last_frame"    , s.frame_count)  << ",\n";
  ss << "\"metadata\" : " << s.metadata << "\n},\n";
  // ss << "\"metadata\" : " << s.metadata << ",\n";

  ss << "\"players\" : [\n";
  for(unsigned p = 0; p < 8; ++p) {
    unsigned pp = (p % 4);
    if(p > 3 && s.player[pp].ext_char_id != 0x0E) {  //If we're not Ice climbers
      if (p == 7) {
        ss << SPACE[ILEV] << "{}\n";
      } else {
        ss << SPACE[ILEV] << "{},\n";
      }
      continue;
    }

    ss << SPACE[ILEV] << "{\n";
    ss << JUIN(2,"player_id"   ,pp)                            << ",\n";
    ss << JUIN(2,"is_follower" ,p > 3)                         << ",\n";
    ss << JUIN(2,"ext_char_id" ,s.player[pp].ext_char_id)      << ",\n";
    ss << JUIN(2,"player_type" ,s.player[pp].player_type)      << ",\n";
    ss << JUIN(2,"start_stocks",s.player[pp].start_stocks)     << ",\n";
    ss << JUIN(2,"color"       ,s.player[pp].color)            << ",\n";
    ss << JUIN(2,"team_id"     ,s.player[pp].team_id)          << ",\n";
    ss << JUIN(2,"dash_back"   ,s.player[pp].dash_back)        << ",\n";
    ss << JUIN(2,"shield_drop" ,s.player[pp].shield_drop)      << ",\n";
    ss << JSTR(2,"tag"         ,escape_json(s.player[pp].tag)) << ",\n";

    if (s.player[p].player_type == 3) {
      ss << SPACE[ILEV*2] << "\"frames\" : []\n";
    } else {
      ss << SPACE[ILEV*2] << "\"frames\" : [\n";
      for(int f = 0; f < total_frames; ++f) {
        ss << SPACE[ILEV*3] << "{";

        int a = 0; //True for only the first thing output per line
        if (CHANGED(follower))
          ss << JEND(a) << JUIN(3,"follower"      ,s.player[p].frame[f].follower);
        if (CHANGED(seed))
          ss << JEND(a) << JUIN(3,"seed"          ,s.player[p].frame[f].seed);
        if (CHANGED(action_pre))
          ss << JEND(a) << JUIN(3,"action_pre"    ,s.player[p].frame[f].action_pre);
        if (CHANGED(pos_x_pre))
          ss << JEND(a) << JFLT(3,"pos_x_pre"     ,s.player[p].frame[f].pos_x_pre);
        if (CHANGED(pos_y_pre))
          ss << JEND(a) << JFLT(3,"pos_y_pre"     ,s.player[p].frame[f].pos_y_pre);
        if (CHANGED(face_dir_pre))
          ss << JEND(a) << JFLT(3,"face_dir_pre"  ,s.player[p].frame[f].face_dir_pre);
        if (CHANGED(joy_x))
          ss << JEND(a) << JFLT(3,"joy_x"         ,s.player[p].frame[f].joy_x);
        if (CHANGED(joy_y))
          ss << JEND(a) << JFLT(3,"joy_y"         ,s.player[p].frame[f].joy_y);
        if (CHANGED(c_x))
          ss << JEND(a) << JFLT(3,"c_x"           ,s.player[p].frame[f].c_x);
        if (CHANGED(c_y))
          ss << JEND(a) << JFLT(3,"c_y"           ,s.player[p].frame[f].c_y);
        if (CHANGED(trigger))
          ss << JEND(a) << JFLT(3,"trigger"       ,s.player[p].frame[f].trigger);
        if (CHANGED(buttons))
          ss << JEND(a) << JUIN(3,"buttons"       ,s.player[p].frame[f].buttons);
        if (CHANGED(phys_l))
          ss << JEND(a) << JFLT(3,"phys_l"        ,s.player[p].frame[f].phys_l);
        if (CHANGED(phys_r))
          ss << JEND(a) << JFLT(3,"phys_r"        ,s.player[p].frame[f].phys_r);
        if (CHANGED(ucf_x))
          ss << JEND(a) << JUIN(3,"ucf_x"         ,s.player[p].frame[f].ucf_x);
        if (CHANGED(percent_pre))
          ss << JEND(a) << JFLT(3,"percent_pre"   ,s.player[p].frame[f].percent_pre);
        if (CHANGED(char_id))
          ss << JEND(a) << JUIN(3,"char_id"       ,s.player[p].frame[f].char_id);
        if (CHANGED(action_post))
          ss << JEND(a) << JUIN(3,"action_post"   ,s.player[p].frame[f].action_post);
        if (CHANGED(pos_x_post))
          ss << JEND(a) << JFLT(3,"pos_x_post"    ,s.player[p].frame[f].pos_x_post);
        if (CHANGED(pos_y_post))
          ss << JEND(a) << JFLT(3,"pos_y_post"    ,s.player[p].frame[f].pos_y_post);
        if (CHANGED(face_dir_post))
          ss << JEND(a) << JFLT(3,"face_dir_post" ,s.player[p].frame[f].face_dir_post);
        if (CHANGED(percent_post))
          ss << JEND(a) << JFLT(3,"percent_post"  ,s.player[p].frame[f].percent_post);
        if (CHANGED(shield))
          ss << JEND(a) << JFLT(3,"shield"        ,s.player[p].frame[f].shield);
        if (CHANGED(hit_with))
          ss << JEND(a) << JUIN(3,"hit_with"      ,s.player[p].frame[f].hit_with);
        if (CHANGED(combo))
          ss << JEND(a) << JUIN(3,"combo"         ,s.player[p].frame[f].combo);
        if (CHANGED(hurt_by))
          ss << JEND(a) << JUIN(3,"hurt_by"       ,s.player[p].frame[f].hurt_by);
        if (CHANGED(stocks))
          ss << JEND(a) << JUIN(3,"stocks"        ,s.player[p].frame[f].stocks);
        if (CHANGED(action_fc))
          ss << JEND(a) << JFLT(3,"action_fc"     ,s.player[p].frame[f].action_fc);
        if (CHANGED(flags_1))
          ss << JEND(a) << JUIN(3,"flags_1"       ,s.player[p].frame[f].flags_1);
        if (CHANGED(flags_2))
          ss << JEND(a) << JUIN(3,"flags_2"       ,s.player[p].frame[f].flags_2);
        if (CHANGED(flags_3))
          ss << JEND(a) << JUIN(3,"flags_3"       ,s.player[p].frame[f].flags_3);
        if (CHANGED(flags_4))
          ss << JEND(a) << JUIN(3,"flags_4"       ,s.player[p].frame[f].flags_4);
        if (CHANGED(flags_5))
          ss << JEND(a) << JUIN(3,"flags_5"       ,s.player[p].frame[f].flags_5);
        if (CHANGED(hitstun))
          ss << JEND(a) << JUIN(3,"hitstun"       ,s.player[p].frame[f].hitstun);
        if (CHANGED(airborne))
          ss << JEND(a) << JUIN(3,"airborne"      ,s.player[p].frame[f].airborne);
        if (CHANGED(ground_id))
          ss << JEND(a) << JUIN(3,"ground_id"     ,s.player[p].frame[f].ground_id);
        if (CHANGED(jumps))
          ss << JEND(a) << JUIN(3,"jumps"         ,s.player[p].frame[f].jumps);
        if (CHANGED(l_cancel))
          ss << JEND(a) << JUIN(3,"l_cancel"      ,s.player[p].frame[f].l_cancel);
        if (CHANGED(alive))
          ss << JEND(a) << JINT(3,"alive"         ,s.player[p].frame[f].alive);
        //Putting this last for a consistent non-comma end
        // ss << JINT(3,"frame"           ,int(f)-123)                        << "\n";

        if (f < total_frames-1) {
          ss << "\n" << SPACE[ILEV*3] << "},\n";
        } else {
          ss << "\n" << SPACE[ILEV*3] << "}\n";
        }
      }
      ss << SPACE[ILEV*2] << "]\n";
    }
    if (p == 7) {
      ss << SPACE[ILEV] << "}\n";
    } else {
      ss << SPACE[ILEV] << "},\n";
    }
  }
  ss << "]\n";

  ss << "}" << std::endl;
  return ss.str();
}

}