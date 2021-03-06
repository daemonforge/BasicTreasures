modded class ChernarusMap  extends ItemMap
{
	protected vector m_TreasureLocation;
		
	
	override void EEOnCECreate(){
		super.EEOnCECreate();
		
		float rand = Math.RandomFloat(0,1);
		if ( !GetGame().IsServer() || rand >  GenerateTreasureSpawnChance() || !BasicMap().CanSpawnNewChest()){
			return;
		}
		
		m_TreasureLocation = this.GenerateTreasureSpawnPos();
		if (IsTreasureAlreadySpawned()){
			return;
		}
		BasicMap().NewChestSpawned();
		

		UndergroundStash stash = UndergroundStash.Cast( GetGame().CreateObject( "UndergroundStash",  m_TreasureLocation));
		if (stash){
			stash.PlaceOnGround(); 
			vector pos = stash.GetPosition();
			vector ori = stash.GetOrientation();
			m_TreasureLocation = pos;
			stash.SetLifetime(604800);
			
			if ( ori[2] == -180 || ori[2] == 180 ){
				ori[2] = 0;
				stash.SetOrientation( ori );
			}
			BasicTreasuresRewardPool TheRewards = GetBasicTreasuresConfig().GetRandomRewardPool();
			ItemBase TheTreasure = ItemBase.Cast( stash.GetInventory().CreateInInventory("SeaChest"));	
			if (TheTreasure){
				for (int i = 0; i < TheRewards.Rewards.Count(); i++){
					EntityAI item = EntityAI.Cast(TheTreasure.GetInventory().CreateInInventory( TheRewards.Rewards.Get(i).Item));
					if (item){
						TStringArray attachments = TheRewards.Rewards.Get(i).ItemAttachments;
						if (attachments && attachments.Count() > 0){
							for (int j = 0;j < attachments.Count(); j++){
								item.GetInventory().CreateInInventory(attachments.Get(j));
							}
						}
					}
				}
			}
		
			ref BasicMapMarker theMarker = new BasicMapMarker("X", pos, "BasicTreasures\\gui\\images\\x.paa", {226, 144, 17}, 200, false);
			theMarker.SetCanEdit(false);
			m_BasicMapMarkerArray.Insert(theMarker);
			
			Print("[Treasure] Map Spawned at: " + GetPosition() + " Stash: " + stash.GetPosition() + " LifeTime: " + stash.GetLifetime());
		}
	}

	
	bool IsTreasureAlreadySpawned(){
		array<Object> objects = new array<Object>;
		array<CargoBase> proxyCargos = new array<CargoBase>;
		GetGame().GetObjectsAtPosition3D(m_TreasureLocation, 2, objects, proxyCargos);
		UndergroundStash stash;
		for (int i = 0; i < objects.Count(); i++){
			if ( Class.CastTo(stash, objects.Get(i)) ){
				return true;
			}
		}
		return false;
	}


	
	override void EEDelete(EntityAI parent){
		if (GetGame()){ //If server is shuting down don't proccess
			bool HasParent = false;
			EntityAI ThisParent = EntityAI.Cast(this.GetParent());
			if (ThisParent){
				HasParent = true;
			}
			float RemainingLife = 0;
			array<Object> objects = new array<Object>;
			array<CargoBase> proxyCargos = new array<CargoBase>;
			GetGame().GetObjectsAtPosition3D(m_TreasureLocation, 3, objects, proxyCargos);
			UndergroundStash stash;
			SeaChest chest;
			for (int i = 0; i < objects.Count(); i++){
				if ( Class.CastTo(stash, objects.Get(i)) ){
					RemainingLife = stash.GetLifetime();
					if (!HasParent && RemainingLife < 400){ //Only delete if the lifetime is less than 400second and it doesn't have a parent
						Print("[Treasure] Map at " + GetPosition() +  "  Deleted Stash: " + m_TreasureLocation + " RemainingLife: " + RemainingLife);
						GetGame().ObjectDelete(stash);
					}
					super.EEDelete(parent);
					return;
				}
				if ( Class.CastTo(chest, objects.Get(i)) ){
					Print("[Treasure] Map at " + GetPosition() +  "  Deleted Chest: " + m_TreasureLocation + " RemainingLife: " + RemainingLife);
					GetGame().ObjectDelete(chest);
				}
			}
		}
		super.EEDelete(parent);
	}

	
	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if ( !super.OnStoreLoad( ctx, version ) )
			return false;
	
		if (!ctx.Read(m_TreasureLocation))
		{
			return false;
		}
		
		return true;
	}
	

	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);
		
		ctx.Write(m_TreasureLocation);
	}
	
	
	override void SyncMapMarkers()
	{
		super.SyncMapMarkers();
		
	}
	
	
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		
	}
	
	
	vector GenerateTreasureSpawnPos(){
		return BasicMap().GetRandomChestSpawn();
	}
	
	
	float GenerateTreasureSpawnChance(){
		return GetBasicTreasuresConfig().ChanceOnSpawn;
	}
	
}