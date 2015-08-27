using UnityEngine;
using System.Collections;

public abstract class Archer : Troop
{
	// Use this for initialization
    protected virtual void Start()
    {
        maxHealth = 50;
        damage = 10;
        range = 3.5f;
        attackSpeed = 0.75f;
        moveTime = 0.20f;
        base.Start();
	}
	 
}
