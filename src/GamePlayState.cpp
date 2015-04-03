#include "AnimatedSprite2D.h"
#include "AnimationSet2D.h"
#include "Camera.h"
#include "CoreEvents.h"
#include "Font.h"
#include "Graphics.h"
#include "Input.h"
#include "Octree.h"
#include "Renderer.h"
#include "ResourceCache.h"
#include "Scene.h"
#include "Sprite.h"
#include "Sprite2D.h"
#include "StaticSprite2D.h"
#include "Texture2D.h"
#include "Text.h"
#include "DebugNew.h"
#include "UIElement.h"
#include "Sprite.h"
#include "Node.h"
#include "UI.h"
#include "Engine.h"
#include "iostream"
#include "RigidBody2D.h"
#include "PhysicsWorld2D.h"
#include "DebugRenderer.h"
#include "CollisionCircle2D.h"
#include "CollisionPolygon2D.h"
#include "File.h"
#include "FileSystem.h"
#include "PhysicsEvents2D.h"
#include "InputEvents.h"
#include "BulletEntity.h"
#include "EnemyEntity.h"
#include "Color.h"
#include "JSONFile.h"
#include "JSONValue.h"
#include "LuaFile.h"
#include "LuaFunction.h"
#include "LuaScript.h"
#include "LuaScriptInstance.h"
#include "TmxFile2D.h"
#include "TileMap2D.h"

#include "GamePlayState.h"
#include "MovablePlatform.h"

// Number of static sprites to draw
static const unsigned NUM_SPRITES = 200;
static const StringHash VAR_MOVESPEED("MoveSpeed");
static const StringHash VAR_ROTATESPEED("RotateSpeed");

GamePlayState::GamePlayState(Context* context) : State(context)
{
    PlayerEntity::RegisterObject(context);
	BulletEntity::RegisterObject(context);
	EnemyEntity::RegisterObject(context);
	MovablePlatform::RegisterObject(context);
	context_->RegisterSubsystem(new LuaScript(context_));
}

GamePlayState::~GamePlayState()
{
}

bool GamePlayState::Begin()
{
    player_hearts = 5;
	// Create the UI content
	CreateUI();
	// Create the scene content
	CreateScene();
	// Setup the viewport for displaying the scene
	SetupViewport();
	// Hook up to the frame update events
	SubscribeToEvents();

	GetSubsystem<Input>()->SetMouseVisible(false);

	return State::Begin();
}

bool GamePlayState::Initialize()
{
	return State::Initialize();
}

void GamePlayState::CreateScene()
{
    scene_ = new Scene(context_);
    scene_->CreateComponent<Octree>();
    scene_->CreateComponent<DebugRenderer>();
    PhysicsWorld2D* physicsWorld = scene_->CreateComponent<PhysicsWorld2D>();
    physicsWorld->SetGravity(Vector2(0,-10));

    // Create camera node
    cameraNode_ = scene_->CreateChild("Camera");
    // Set camera's position
    cameraNode_->SetPosition(Vector3(7.5f, 7.5, -10.0f));

    Camera* camera = cameraNode_->CreateComponent<Camera>();
    camera->SetOrthographic(true);

    Graphics* graphics = GetSubsystem<Graphics>();
    camera->SetOrthoSize((float)graphics->GetHeight() * PIXEL_SIZE);
    //camera->SetZoom(0.7f);

    ResourceCache* cache = GetSubsystem<ResourceCache>();

    nodeWall = scene_->CreateChild("NodoWall");

    XMLFile* nodoXMLFile = cache->GetResource<XMLFile>("Scenes/nodo_map.xml");
    XMLElement nodoXML(nodoXMLFile->GetRoot());
    nodeWall->LoadXML(nodoXML);

    // Create 2D physics world component

    bgNode_ = scene_->CreateChild("Target");
    Sprite2D* bgsprite = cache->GetResource<Sprite2D>("Urho2D/bg.png");
    if (!bgsprite)
        return;

    StaticSprite2D* bgstaticsprite = bgNode_->CreateComponent<StaticSprite2D>();
    bgstaticsprite->SetSprite(bgsprite);
    bgstaticsprite->SetLayer(-65000);
    bgNode_->SetScale(5.0f);

    // Get tmx file
    /*TmxFile2D* tmxFile = cache->GetResource<TmxFile2D>("Maps/nivel1.tmx");
    if (!tmxFile)
        return;

    SharedPtr<Node> tileMapNode(scene_->CreateChild("TileMap"));
    tileMapNode->SetPosition(Vector3(0.0f, 0.0f, -1.0f));

    TileMap2D* tileMap = tileMapNode->CreateComponent<TileMap2D>();
    // Set animation
    tileMap->SetTmxFile(tmxFile);*/

    JSONFile* data = new JSONFile(context_);
    File file(context_, "Data/Scenes/MapNode.json");
    data->Load(file);

    JSONValue rootjson = data->GetRoot();

    JSONValue blocks = rootjson.GetChild("blocks");

    Vector2 position = rootjson.GetVector2("playerpost");

    // Get animation set
    SharedPtr<Node> spriteNode(scene_->CreateChild("Player"));
    spriteNode->SetPosition2D(position);
    player_ = spriteNode->CreateComponent<PlayerEntity>();
    player_->SetHeart(player_hearts);
    spriteNode->SetScale(0.2f);

    /*LuaFile* scriptFile = cache->GetResource<LuaFile>("Scripts/player.lua");
    if (!scriptFile)
        return;
    LuaScriptInstance* instance = spriteNode->CreateComponent<LuaScriptInstance>();
    instance->CreateObject(scriptFile, "Rotator");

    /* platform*/
    Node* platformnode(scene_->CreateChild("Platform"));

    MovablePlatform* movplat = platformnode->CreateComponent<MovablePlatform>();
    movplat->SetPoints(Vector2(8,18), Vector2(10.8f,18));

    /*end platform*/

    SharedPtr<Node> enemy(scene_->CreateChild("Enemy"));
    enemy->SetScale(0.3f);
    enemy->SetPosition2D(Vector2(13,18.5f));
    EnemyEntity* enemy_ = enemy->CreateComponent<EnemyEntity>();
}

void GamePlayState::CreateUI()
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();
    // Create a draggable Fish button
    Sprite* casco = new Sprite(context_);
    casco->SetTexture(cache->GetResource<Texture2D>("Urho2D/casco.png")); // Set texture
    casco->SetSize(75, 100);
    casco->SetPosition(20, 16);
    casco->SetName("Casco");
    GetSubsystem<UI>()->GetRoot()->AddChild(casco);
    CreateHearts();
}

void GamePlayState::CreateHearts()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    for(int i = 0; i < player_hearts; i++)
    {
        Sprite* heart = new Sprite(context_);
        heart->SetTexture(cache->GetResource<Texture2D>("Urho2D/heart.png")); // Set texture
        heart->SetSize(40, 37);
        heart->SetPosition(100+(i*45), 50);
        heart->SetName("heart");
        GetSubsystem<UI>()->GetRoot()->AddChild(heart);
        CountHeart.Push(heart);
    }
}

void GamePlayState::ReduceHearts(StringHash eventType, VariantMap& eventData)
{
    using namespace PlayerHurt;

    isDead = eventData[P_ISDEAD].GetBool();

    if(!CountHeart.Empty())
    {
        Sprite* heart = CountHeart.Back();
        GetSubsystem<UI>()->GetRoot()->RemoveChild(heart);
        heart->Remove();
        CountHeart.Pop();
    }
}

void GamePlayState::SetupViewport()
{
    Renderer* renderer = GetSubsystem<Renderer>();

    renderer->SetNumViewports(1);
    // Set up a viewport to the Renderer subsystem so that the 3D scene can be seen
    SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraNode_->GetComponent<Camera>()));
    renderer->SetViewport(0, viewport);
}

void GamePlayState::SubscribeToEvents()
{
    // Subscribe HandleUpdate() function for processing update events
    SubscribeToEvent(E_UPDATE, HANDLER(GamePlayState, HandleUpdate));
    SubscribeToEvent(E_PHYSICSBEGINCONTACT2D, HANDLER(GamePlayState, HandleBeginContact));
    SubscribeToEvent(E_PHYSICSENDCONTACT2D, HANDLER(GamePlayState, HandleEndContact));
    SubscribeToEvent(E_PLAYERHURT, HANDLER(GamePlayState, ReduceHearts));
    /*SubscribeToEvent(E_JOYSTICKBUTTONDOWN, HANDLER(GamePlayState, HandleJoystickButtonDownPressed));
    SubscribeToEvent(E_JOYSTICKAXISMOVE, HANDLER(GamePlayState, HandleJoystickAxisMove));*/
}



void GamePlayState::HandleBeginContact(StringHash eventType, VariantMap& eventData)
{
    using namespace PhysicsBeginContact2D;
    Node* nodeA = static_cast<Node*>(eventData[P_NODEA].GetPtr());
    Node* nodeB = static_cast<Node*>(eventData[P_NODEB].GetPtr());

    if(nodeA->GetName() == "e_bullet" && nodeB->GetName() == "Player")
    {
       RemoveBulletList.Push(nodeA);
       player_->SetHurt(nodeA->GetPosition2D());
    }

    if(nodeB->GetName() == "e_bullet" && nodeA->GetName() == "Player")
    {
        RemoveBulletList.Push(nodeB);
       player_->SetHurt(nodeB->GetPosition2D());
    }


    if(nodeA->GetName() == "e_bullet" && nodeB->GetName() == "Platform")
        RemoveBulletList.Push(nodeA);
    if(nodeB->GetName() == "e_bullet" && nodeA->GetName() == "Platform")
        RemoveBulletList.Push(nodeB);

    if(nodeA->GetName() == "p_bullet" && nodeB->GetName() == "Enemy")
    {
        nodeB->GetComponent<EnemyEntity>()->SetHurt(nodeA->GetPosition2D());
    }
    if(nodeB->GetName() == "p_bullet" && nodeA->GetName() == "Enemy")
    {
        nodeA->GetComponent<EnemyEntity>()->SetHurt(nodeB->GetPosition2D());
    }
}

void GamePlayState::HandleEndContact(StringHash eventType, VariantMap& eventData)
{
    using namespace PhysicsEndContact2D;
    Node* nodeA = static_cast<Node*>(eventData[P_NODEA].GetPtr());
    Node* nodeB = static_cast<Node*>(eventData[P_NODEB].GetPtr());
}

void GamePlayState::HandleJoystickAxisMove(StringHash eventType, VariantMap& eventData)
{
    using namespace JoystickAxisMove;

    int button = eventData[P_AXIS].GetInt();
    float position = eventData[P_POSITION].GetFloat();
    std::cout<<button<<" - "<<position<<std::endl;
}

void GamePlayState::HandleJoystickButtonDownPressed(StringHash eventType, VariantMap& eventData)
{
    using namespace JoystickButtonDown;

    int button = eventData[P_BUTTON].GetInt();
    std::cout<<button<<std::endl;
}

void GamePlayState::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace Update;

    // Take the frame time step, which is stored as a float
    float timeStep = eventData[P_TIMESTEP].GetFloat();

    Graphics* graphics = GetSubsystem<Graphics>();
    Input* input = GetSubsystem<Input>();

    if (GetSubsystem<UI>()->GetFocusElement())
        return;

    Vector2 posplayer(player_->GetNode()->GetPosition2D());

    if (player_ && !isDead)
    {
        // Clear previous controls
        player_->controls_.Set(CTRL_DOWN | CTRL_LEFT | CTRL_RIGHT | LOOK_LEFT, false);
        //keyboard
        //player_->controls_.Set(CTRL_DOWN, input->GetKeyDown(KEY_DOWN));
        player_->controls_.Set(CTRL_LEFT, input->GetKeyDown(KEY_LEFT) || input->GetJoystickByIndex(0)->GetButtonDown(CONTROLLER_BUTTON_DPAD_LEFT));
        player_->controls_.Set(CTRL_RIGHT, input->GetKeyDown(KEY_RIGHT) || input->GetJoystickByIndex(0)->GetButtonDown(CONTROLLER_BUTTON_DPAD_RIGHT));

        if(input->GetKeyPress(KEY_SPACE) || input->GetJoystickByIndex(0)->GetButtonPress(CONTROLLER_BUTTON_A))
            player_->SetJump();

        if(input->GetKeyPress('C') || input->GetJoystickByIndex(0)->GetButtonPress(CONTROLLER_BUTTON_X))
            player_->SetAtack();
    }
    PhysicsWorld2D* physicsWorld = scene_->GetComponent<PhysicsWorld2D>();
    physicsWorld->DrawDebugGeometry();

    cameraNode_->Translate2D((posplayer - cameraNode_->GetPosition2D())*0.05);
    bgNode_->SetPosition2D(cameraNode_->GetPosition2D());

    while(!RemoveBulletList.Empty())
    {
        Node* current = RemoveBulletList.Back();
        current->Remove();
        RemoveBulletList.Pop();
    }

    if (input->GetKeyPress('P') || input->GetJoystickByIndex(0)->GetButtonPress(CONTROLLER_BUTTON_START))
    {
        isPause = !isPause;
        scene_->SetUpdateEnabled(isPause);
    }

    if (input->GetKeyPress('R'))
    {
        stateManager_->PopStack();
		stateManager_->PushToStack("GamePlayState");
    }

    if (input->GetKeyPress('O'))
    {
        stateManager_->PopStack();
		stateManager_->PushToStack("MainMenu");
    }

}

void GamePlayState::End()
{
    scene_.Reset();
	cameraNode_.Reset();
	if (GetSubsystem<UI>())
	{
		GetSubsystem<UI>()->Clear();
	}
    UnsubscribeFromAllEvents();
	State::End();
}
