import "config.h";
import "game/entity.h";
import "lib/gi.h";
import "game/util.h";
import "game/weaponry.h";
import "game/ballistics.h";
import "lib/math/random.h";
import "game/combat.h";
import "lib/math/vector.h";

constexpr spawn_flag GRENADE_IS_HAND = (spawn_flag) 1;
constexpr spawn_flag GRENADE_IS_HELD = (spawn_flag) 2;

void Grenade_Explode(entity &ent)
{
	vector			origin;
	means_of_death	mod;
#ifdef SINGLE_PLAYER

	if (ent.owner->is_client())
		PlayerNoise(ent.owner, ent.s.origin, PNOISE_IMPACT);
#endif

	//FIXME: if we are onground then raise our Z just a bit since we are a point?
	if (ent.enemy.has_value())
	{
		float	points;
		vector	v;
		vector dir;

		v = ent.enemy->mins + ent.enemy->maxs;
		v = ent.enemy->s.origin + (0.5f * v);
		v = ent.s.origin - v;
		points = ent.dmg - 0.5f * VectorLength(v);
		dir = ent.enemy->s.origin - ent.s.origin;
		if (ent.spawnflags & GRENADE_IS_HAND)
			mod = MOD_HANDGRENADE;
		else
			mod = MOD_GRENADE;
		T_Damage(ent.enemy, ent, ent.owner, dir, ent.s.origin, vec3_origin, (int32_t) points, (int32_t) points, DAMAGE_RADIUS, mod);
	}

	if (ent.spawnflags & GRENADE_IS_HELD)
		mod = MOD_HELD_GRENADE;
	else if (ent.spawnflags & GRENADE_IS_HAND)
		mod = MOD_HG_SPLASH;
	else
		mod = MOD_G_SPLASH;
	T_RadiusDamage(ent, ent.owner, (float) ent.dmg, ent.enemy, ent.dmg_radius, mod);

	origin = ent.s.origin + (-0.02f * ent.velocity);
	gi.WriteByte(svc_temp_entity);
	if (ent.waterlevel)
	{
		if (ent.groundentity.has_value())
			gi.WriteByte(TE_GRENADE_EXPLOSION_WATER);
		else
			gi.WriteByte(TE_ROCKET_EXPLOSION_WATER);
	}
	else
	{
		if (ent.groundentity.has_value())
			gi.WriteByte(TE_GRENADE_EXPLOSION);
		else
			gi.WriteByte(TE_ROCKET_EXPLOSION);
	}
	gi.WritePosition(origin);
	gi.multicast(ent.s.origin, MULTICAST_PHS);

	G_FreeEdict(ent);
}

REGISTER_SAVABLE_FUNCTION(Grenade_Explode);

void Grenade_Touch(entity &ent, entity &other, vector, const surface &surf)
{
	if (other == ent.owner)
		return;

	if (surf.flags & SURF_SKY)
	{
		G_FreeEdict(ent);
		return;
	}

	if (!other.takedamage)
	{
		if (ent.spawnflags & 1)
		{
			if (random() > 0.5f)
				gi.sound(ent, CHAN_VOICE, gi.soundindex("weapons/hgrenb1a.wav"), 1, ATTN_NORM, 0);
			else
				gi.sound(ent, CHAN_VOICE, gi.soundindex("weapons/hgrenb2a.wav"), 1, ATTN_NORM, 0);
		}
		else
			gi.sound(ent, CHAN_VOICE, gi.soundindex("weapons/grenlb1b.wav"), 1, ATTN_NORM, 0);

		return;
	}

	ent.enemy = other;
	Grenade_Explode(ent);
}

REGISTER_SAVABLE_FUNCTION(Grenade_Touch);

void fire_grenade(entity &self, vector start, vector aimdir, int32_t damage, int32_t speed, float timer, float damage_radius)
{
	vector	dir;
	vector	forward, right, up;
	float   scale;

	dir = vectoangles(aimdir);
	AngleVectors(dir, &forward, &right, &up);

	entity &grenade = G_Spawn();
	grenade.s.origin = start;
	grenade.velocity = aimdir * speed;
	scale = random(190.f, 210.f);
	grenade.velocity += (scale * up);
	scale = random(-10.f, 10.f);
	grenade.velocity += (scale * right);
	grenade.avelocity = { 300, 300, 300 };
	grenade.movetype = MOVETYPE_BOUNCE;
	grenade.clipmask = MASK_SHOT;
	grenade.solid = SOLID_BBOX;
	grenade.s.effects |= EF_GRENADE;
	grenade.mins = vec3_origin;
	grenade.maxs = vec3_origin;
	grenade.s.modelindex = gi.modelindex("models/objects/grenade/tris.md2");
	grenade.owner = self;
	grenade.touch = Grenade_Touch_savable;
	grenade.nextthink = level.framenum + (gtime) (timer * BASE_FRAMERATE);
	grenade.think = Grenade_Explode_savable;
	grenade.dmg = damage;
	grenade.dmg_radius = damage_radius;
	grenade.type = ET_GRENADE;

	gi.linkentity(grenade);
}

void fire_grenade2(entity &self, vector start, vector aimdir, int32_t damage, int32_t speed, float timer, float damage_radius, bool held)
{
	vector	dir;
	vector	forward, right, up;
	float   scale;

	dir = vectoangles(aimdir);
	AngleVectors(dir, &forward, &right, &up);

	entity &grenade = G_Spawn();
	grenade.s.origin = start;
	grenade.velocity = aimdir * speed;
	scale = random(190.f, 210.f);
	grenade.velocity += (scale * up);
	scale = random(-10.f, 10.f);
	grenade.velocity += (scale * right);
	grenade.avelocity = { 300, 300, 300 };
	grenade.movetype = MOVETYPE_BOUNCE;
	grenade.clipmask = MASK_SHOT;
	grenade.solid = SOLID_BBOX;
	grenade.s.effects |= EF_GRENADE;
	grenade.mins = vec3_origin;
	grenade.maxs = vec3_origin;
	grenade.s.modelindex = gi.modelindex("models/objects/grenade2/tris.md2");
	grenade.owner = self;
	grenade.touch = Grenade_Touch_savable;
	grenade.nextthink = level.framenum + (gtime) (timer * BASE_FRAMERATE);
	grenade.think = Grenade_Explode_savable;
	grenade.dmg = damage;
	grenade.dmg_radius = damage_radius;
	grenade.type = ET_HANDGRENADE;
	grenade.spawnflags = GRENADE_IS_HAND;
	if (held)
		grenade.spawnflags |= GRENADE_IS_HELD;
	grenade.s.sound = gi.soundindex("weapons/hgrenc1b.wav");

	if (timer <= 0.0f)
		Grenade_Explode(grenade);
	else
	{
		gi.sound(self, CHAN_WEAPON, gi.soundindex("weapons/hgrent1a.wav"), 1, ATTN_NORM, 0);
		gi.linkentity(grenade);
	}
}
