enum bone : int {
    Head = 110,
    Neck = 67,
    Chest = 66,
    Pelvis = 2,
    LShoulder = 9,
    LElbow = 10,
    LHand = 11,
    RShoulder = 38,
    RElbow = 39,
    RHand = 40,
    LHip = 71,
    LKnee = 72,
    LFoot = 73,
    RHip = 78,
    RKnee = 79,
    RFoot = 82,
    Root = 0
};

enum offset {
    UWorld = 0x1683A738,
    GameState = 0x180, // https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=UWorld&member=GameState
    PlayerArray = 0x2c0, // https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=AGameStateBase&member=PlayerArray
    GameInstance = 0x1F8, // https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=UWorld&member=OwningGameInstance
    LocalPlayers = 0x38, // https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=UGameInstance&member=LocalPlayers
    PlayerController = 0x30,
    LocalPawn = 0x350, // https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=APlayerController&member=AcknowledgedPawn 
    PlayerState = 0x2C8, // https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=AController&member=PlayerState
    RootComponent = 0x1B0, // https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=AActor&member=RootComponent
    Mesh = 0x328, //https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=ACharacter&member=Mesh
    TeamIndex = 0x1259,
    PawnPrivate = 0x320, // https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=APlayerState&member=PawnPrivate
    RelativeLocation = 0x138, // https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=USceneComponent&member=RelativeLocation
    BoneArray = 0x5A8,
    ComponentToWorld = 0x1E0,
    AmmoCount = 0x10bc, // https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=AFortWeapon&member=AmmoCount
    CurrentWeapon = 0xA80, // https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=AFortPawn&member=CurrentWeapon
    WeaponData = 0x568, // https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=AFortWeapon&member=WeaponData
};
