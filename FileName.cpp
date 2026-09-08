#include<graphics.h>
#include <string>
#include<vector>//动态数组
int idx_current_anim = 0;
const int PLAYER_ANIM_NUM = 6;
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

inline void putimage_alpha(int x,int y,IMAGE* img)
{
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h,
		GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER,0,255,AC_SRC_ALPHA });
}
class Animation //图片处理
{
public:
	Animation(LPCTSTR path,int num,int interval)//路径相同部分，图片数，帧间隔
	{
		TCHAR path_file[256];
		interval_ms = interval;

		for (size_t i = 0; i < num; i++)
		{
			_stprintf_s(path_file, path, i);

			IMAGE* frame = new IMAGE();
			loadimage(frame, path_file);//先指针，后路径，导入即载入
			frame_list.push_back(frame);//放入容器
		}
	}																	
	~Animation()
	{
		for (size_t i = 0; i < frame_list.size(); i++)//遍历删除申请的空间,避免内存溢出
			delete frame_list[i];
	}
	void Play(int x,int y,int delta)//坐标，间隔
	{
		timer += delta;
		//动画载入与播放
		if (timer >= interval_ms)
		{
			idx_frame = (idx_frame + 1 ) % frame_list.size();//(idx_frame++) %= frame_list.size();
			timer = 0;
		}
		putimage_alpha(x, y, frame_list[idx_frame]);
	}
private:
	int timer = 0;//动画计时器
	int idx_frame = 0;//动画帧索引
	int interval_ms = 0;
	std::vector<IMAGE*> frame_list;//类似于动态长度的数组
};

IMAGE img_player_left[PLAYER_ANIM_NUM];
IMAGE img_player_right[PLAYER_ANIM_NUM];

class Player
{
public:
	Player()
	{
		loadimage(&img_shadow, _T("img/shadow_player.png"));//
		anim_left = new Animation(_T("img/player_left_%d.png"), 6, 45);
		anim_right = new Animation(_T("img/player_right_%d.png"), 6, 45);
	}

	~Player()
	{
		delete anim_left;
		delete anim_right;
	}

	void ProcessEvent(const ExMessage& msg){
		switch (msg.message)
		{
		case WM_KEYDOWN:
			switch (msg.vkcode) {
			case VK_DOWN:
				is_move_down = true; break;
			case VK_UP:
				is_move_up = true; break;
			case VK_RIGHT:
				is_move_right = true; break;
			case VK_LEFT:
				is_move_left = true; break;
			}
			break;
		case WM_KEYUP:
				switch (msg.vkcode) {
				case VK_DOWN:
					is_move_down = false; break;
				case VK_UP:
					is_move_up = false; break;
				case VK_RIGHT:
					is_move_right = false; break;
				case VK_LEFT:
					is_move_left = false; break;
				}
				break;
		}
	}

	void Move() {
		//移动标准化
		int dir_x = is_move_right - is_move_left;
		int dir_y = is_move_down - is_move_up;
		double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);
		if (len_dir != 0)
		{
			double normalized_x = dir_x / len_dir;
			double normalized_y = dir_y / len_dir;
			position.x += (int)(PLAYER_SPEED * normalized_x);
			position.y += (int)(PLAYER_SPEED * normalized_y);
		}
		//边界设置
		if (position.x < -(PLAYER_WIDTH / 2))position.x = -(PLAYER_WIDTH / 2);
		if (position.y < -(PLAYER_HEIGHT / 2))position.y = -(PLAYER_HEIGHT / 2);
		if (position.x > WINDOW_WIDTH - (PLAYER_WIDTH / 2))position.x = WINDOW_WIDTH - (PLAYER_WIDTH / 2);
		if (position.y > WINDOW_HEIGHT - (PLAYER_HEIGHT / 2))position.y = WINDOW_HEIGHT - (PLAYER_HEIGHT / 2);

		if (dir_x < 0)
			facing_left = true;
		else if (dir_x > 0)
			facing_left = false;
	}

	void Draw(int delta){
		int pos_shadow_x = position.x + (PLAYER_WIDTH / 2 - SHADOW_WIDTH / 2);
		int pos_shadow_y = PLAYER_HEIGHT + position.y - 8;
		putimage_alpha(pos_shadow_x, pos_shadow_y, &img_shadow);

		

		if (facing_left)
			anim_left->Play(position.x, position.y, delta);
		else
			anim_right->Play(position.x, position.y, delta);
	}

	const POINT& GetPosition()const
	{
		return position;
	}
private:
	const int PLAYER_WIDTH = 80;//玩家宽
	const int PLAYER_HEIGHT = 80;//玩家高
	const int SHADOW_WIDTH = 32;//阴影
	const int PLAYER_SPEED = 10;

private:
	
	IMAGE img_shadow;
	Animation* anim_left;
	Animation* anim_right;
	POINT position = { 500,500 };
	bool is_move_down = false;
	bool is_move_up = false;
	bool is_move_left = false;
	bool is_move_right = false;
	bool facing_left = false;
	int dir_x = is_move_right - is_move_left;
	int dir_y = is_move_down - is_move_up;
};


class Bullet {
public:
	POINT position = { 0,0 };
public:
	Bullet() = default;
	~Bullet() = default;

	void Draw() const {
		setlinecolor(RGB(255, 155, 50));
		setfillcolor(RGB(200, 75, 10));
		fillcircle(position.x, position.y, RADIUS);
	}
private:
	const int RADIUS = 10;
};


class Enemy
{
public:
	Enemy()
	{
		loadimage(&img_shadow, _T("img/shadow_enemy.png"));//
		anim_left = new Animation(_T("img/enemy_left_%d.png"), 6, 45);
		anim_right = new Animation(_T("img/enemy_right_%d.png"), 6, 45);
		
		//生成边界
		enum class SpawnEdge
		{
			Up = 0,
		    Down,
			Left,
			Right
		};

		//随机位置
		SpawnEdge edge = (SpawnEdge)(rand() % 4);
		switch (edge)
		{
		case SpawnEdge::Up:
			position.x = rand() % WINDOW_WIDTH;
			position.y = -FRAME_HEIGHT;
			break;
		case SpawnEdge::Down:
			position.x = rand() % WINDOW_WIDTH;
			position.y = WINDOW_HEIGHT;
			break;
		case SpawnEdge::Left:
			position.x = -FRAME_WIDTH;
			position.y = rand() % WINDOW_HEIGHT;
			break;
		case SpawnEdge::Right:
			position.x = WINDOW_WIDTH;
			position.y = rand() % WINDOW_HEIGHT;
			break;

		}
	}
	bool CheckBulletCollision(const Bullet& bullet)//子弹碰撞（敌人）
	{
		//点与矩形
		bool is_ovrlap_x = bullet.position.x >= position.x && bullet.position.x <= position.x + FRAME_WIDTH;
		bool is_ovrlap_y = bullet.position.y >= position.y && bullet.position.y <= position.y + FRAME_HEIGHT;

		return is_ovrlap_x && is_ovrlap_y;
	}
	bool CheckPlayerCollision(const Player& Player)//玩家碰撞（敌人）
	{  //敌人为点，点与矩形
		POINT check_position = { position.x + FRAME_WIDTH / 2, position.y + FRAME_HEIGHT / 2 };//敌人中心坐标
		bool is_ovrlap_x = check_position.x >= Player.GetPosition().x && check_position.x <= Player.GetPosition().x + 50;
		bool is_ovrlap_y = check_position.y >= Player.GetPosition().y && check_position.y <= Player.GetPosition().y + 50;
		return is_ovrlap_x && is_ovrlap_y;
	}
	void Move(const Player& player)//敌人移动
	{
		const POINT& player_position = player.GetPosition();
		int dir_x = player_position.x - position.x;
		int dir_y = player_position.y - position.y;
		double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);
		if (len_dir != 0)
		{
			double normalized_x = dir_x / len_dir;
			double normalized_y = dir_y / len_dir;
			position.x += (int)(SPEED * normalized_x);
			position.y += (int)(SPEED * normalized_y);
		}
		if (dir_x < 0)
			facing_left = true;
		else if (dir_x > 0)
			facing_left = false;
	}
	void Draw(int delta) {
		int pos_shadow_x = position.x + (FRAME_WIDTH / 2 - SHADOW_WIDTH / 2);
		int pos_shadow_y = FRAME_HEIGHT + position.y -25;
		putimage_alpha(pos_shadow_x, pos_shadow_y, &img_shadow);

		if (facing_left)
			anim_left->Play(position.x, position.y, delta);
		else
			anim_right->Play(position.x, position.y, delta);
	}
	~Enemy()
	{
		delete anim_left;
		delete anim_right;
	}

	

	
private:
	const int FRAME_WIDTH = 80;//宽
	const int FRAME_HEIGHT = 80;//高
	const int SHADOW_WIDTH = 48;//阴影
	const int SPEED = 5;

private:

	IMAGE img_shadow;
	Animation* anim_left;
	Animation* anim_right;
	POINT position = { 0,0 };
	bool facing_left = false;
};

void TryGenerateEnemy(std::vector<Enemy*>& enemy_list)
{
	const int INTERVAL = 100;
	static int counter = 0;
	if (++counter % INTERVAL == 0)
		enemy_list.push_back(new Enemy());
}

// 正交底片（透明度）
#pragma comment(lib,"MSIMG32.LIB")


//Animation anim_left(_T("img/player_left_%d.png"), 6, 45);// 注意文件名
//Animation anim_right(_T("img/player_right_%d.png"), 6, 45);


/*void DrawPlayer(int delta, int dir_x)
{
	int pos_shadow_x = position.x + (PLAYER_WIDTH/ 2 - SHADOW_WIDTH/2);
	int pos_shadow_y = PLAYER_HEIGHT + position.y - 8;
	putimage_alpha(pos_shadow_x, pos_shadow_y, &img_shadow);
	static bool facing_left = false;
	if (dir_x > 0)
		facing_left = true;
	else if (dir_x < 0)
		facing_left = false;

	if (facing_left)
		anim_left.Play(position.x, position.y, delta);
	else
		anim_right.Play(position.x, position.y, delta);
}*/
/*void LoadAnimation()//加载动画帧
{
	for(size_t i = 0;i < PLAYER_ANIM_NUM;i++)
	{
		std::wstring path = L"img/player_left_" + std::to_wstring(i) + L".png";
		loadimage(&img_player_left[i], path.c_str());
	}
	for (size_t i = 0; i < PLAYER_ANIM_NUM; i++)
	{
		std::wstring path = L"img/player_right_" + std::to_wstring(i) + L".png";
		loadimage(&img_player_right[i], path.c_str());
	}
}*/

void UpdateBullets(std::vector<Bullet>& bullet_list, const Player& player) //子弹更新  （子弹序列，跟随目标）
{
	const double RADIAL_SPEED = 0.0045;//径向速度
	const double TANAENT_SPEED = 0.0045;//切向速度
	double radian_interval = 2 * 3.14159 / bullet_list.size();//弧度间隔
	POINT player_position = player.GetPosition();
	double radius = 100 + 25 * sin(GetTickCount() * RADIAL_SPEED);
	for (size_t i = 0; i < bullet_list.size(); i++) 
	{
		double radian = GetTickCount() * TANAENT_SPEED + radian_interval * i; //子弹当前弧度
		bullet_list[i].position.x = player_position.x + 40 + (int)(radius * sin(radian));
		bullet_list[i].position.y = player_position.y + 40 + (int)(radius * cos(radian));

	}

}

int main()
{
	initgraph(1280, 720);//初始化

	bool running = true;

	ExMessage msg;
	IMAGE img_background;
	//LoadAnimation();//加载动画帧
	loadimage(&img_background, _T("img/background.png"));//加载背景
	

	BeginBatchDraw();

	Player player;
	std::vector<Enemy*> enemy_list;
	std::vector<Bullet> bullet_list(3);


	while (running)
	{
		DWORD start_time = GetTickCount();
		while (peekmessage(&msg))//消息处理
		{ 
			/*if (msg.message == WM_KEYDOWN)
			
				switch (msg.vkcode) {
				case VK_DOWN:
					is_move_down = true; break;
				case VK_UP:
					is_move_up = true; break;
				case VK_RIGHT:
					is_move_right = true; break;
				case VK_LEFT:
					is_move_left = true; break;
				}
			if (msg.message == WM_KEYUP)

				switch (msg.vkcode) {
				case VK_DOWN:
					is_move_down = false; break;
				case VK_UP:
					is_move_up = false; break;
				case VK_RIGHT:
					is_move_right = false; break;
				case VK_LEFT:
					is_move_left = false; break;
				}*/
			/*if (is_move_down)position.y += PLAYER_SPEED;
			if (is_move_up)  position.y -= PLAYER_SPEED;
			if (is_move_left)position.x -= PLAYER_SPEED;
			if (is_move_right)position.x += PLAYER_SPEED;*/
			player.ProcessEvent(msg);
		}

		player.Move();
		UpdateBullets(bullet_list, player);
		TryGenerateEnemy(enemy_list);
		for (Enemy* enemy : enemy_list)
			enemy->Move(player);

		/*static int counter = 0;
		if (++counter % 5 == 0)//游戏帧
			idx_current_anim++;//动画帧
		idx_current_anim = idx_current_anim % PLAYER_ANIM_NUM;*/
		

		//敌我碰撞
		for (Enemy* enemy : enemy_list)
		{
			if (enemy->CheckPlayerCollision(player))

			{
				MessageBox(GetHWnd(), _T("派蒙寄了"), _T("战败"), MB_OK);
				running = false;
				break;
			}
		}
		cleardevice(); 

		putimage(0, 0, &img_background);
		player.Draw(1000 / 144);
		//DrawPlayer(1000 / 144, is_move_left - is_move_right);
		for (Enemy* enemy : enemy_list)//绘制敌人
			enemy->Draw(1000 / 144);
		for (Bullet bullet : bullet_list)//绘制子弹
			bullet.Draw();
		//putimage_alpha(position.x, position.y, &img_player_left[idx_current_anim]);


		FlushBatchDraw();

		DWORD end_time = GetTickCount();
		DWORD delta_time = end_time - start_time;
		if (delta_time < 1000 / 144)
		{
			Sleep(1000 / 144 - delta_time);
		}
	}
	EndBatchDraw();

	return 0;
}