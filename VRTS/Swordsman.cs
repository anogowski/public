using UnityEngine;
using System.Collections;

public class Swordsman : Troop 
{

    protected virtual void Start() 
    {
        maxHealth = 125;
        damage = 25;
        range = 1.5f;
        attackSpeed = 0.5f;
        moveTime = 0.25f;

      base.Start();
	}

    protected virtual void Update()
    {
        base.Update();
    }
}
