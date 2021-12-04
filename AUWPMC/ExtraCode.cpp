//if (Settings.Misc.FOVChanger && Cache.LocalActor);
//{
//    auto ViewInfo = (APlayerCameraManager*)Cache.LocalActor;
//    Cache.LPFOV = Settings.Misc.FOVChangerSlider * 1000000 + 1055158912;
//}

//if (Settings.Misc.RapidFire)
//{
//    Cache.LocalActor->CurrentWeapon->LastNotifyShotTime = 0;
//}

// if (Settings.Misc.LongArms)
// {
//     Cache.LPC->MaxUseDistance = 5000;
 //}

            //if (Settings.Misc.PlayerCount)
            //{
            //    auto OnlineSesh = Actor->OnlineSessionEntryButton();
            //    std::string OnlinePlayers = OnlineSesh->NumPlayers;
            //}

            //if (Settings.Misc.RhinoCharge)
            //{
            //    auto RiddenDino = Cache.LocalActor->GetBasedOrSeatingOnDino();
            //      if (RiddenDino)
            //    {
                    //auto DinoChar = (APrimalDinoCharacter*)Actor;
            //        RiddenDino->ChargeSpeedMultiplier = 50000;
            //    }
            //}

            //if (Settings.Misc.PopcornEnable)
            //{
            //    auto Inv = Cache.LocalActor->MyInventoryComponent;
            //    if (Inv)
            //    {
            //        for (int i = 0; 1 < Inv->InventoryItems.Count; i++)
            //        {
            //            auto Item = Inv->InventoryItems[i];
            //            if (Item)
             //           {
                           // Inv->STATIC_StaticDropNewItemWithInfo(Actor, Item,)
             //           }
             //       }
             //   }
            //}//

        //ImGui::Checkbox(("Rhino Charge"), &Settings.Misc.RhinoCharge);
        //ImGui::Checkbox(("Long Arms"), &Settings.Misc.LongArms);
        //ImGui::Checkbox(("Hide Fish"), &Settings.Visuals.HideFish);
        //ImGui::Checkbox(("Fov Changer"), &Settings.Misc.FOVChanger);
        //ImGui::SliderFloat(("FOV Changer Percent"), &Settings.Misc.FOVChangerSlider, 0.f, 500.f);
        //ImGui::Checkbox(("Rapid Fire"), &Settings.Misc.RapidFire);
        //ImGui::Checkbox(("Speed Hack"), &Settings.Misc.SpeedHacks);
        //ImGui::SliderFloat(("Speed %"), &Settings.Misc.NewSpeed, 0.f, 3.f);


        //ImGui::Columns(2);
        //ImGui::SetColumnOffset(1, 100);
        //ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        //ImGui::BeginChild("##Columns", ImVec2(100, 400), true, ImGuiWindowFlags_NoDecoration);
        //{
        //    if (ImGui::Button(("Visuals"), ImVec2(100, 50))) { Tab = 1; }
        //    if (ImGui::Button(("Aim"), ImVec2(100, 50))) { Tab = 2; }
        //    if (ImGui::Button(("Misc"), ImVec2(100, 50))) { Tab = 3; }
        //    if (ImGui::Button(("Colors"), ImVec2(100, 50))) { Tab = 4; }
        //    if (ImGui::Button(("Home"), ImVec2(100, 50))) { Tab = 5; }
        //}
        //ImGui::EndChild();
        //ImGui::EndColumns();

            // if (Settings.Misc.SpeedHacks)
            // {
            //     auto CharMove = Cache.LocalActor->CharacterMovement;
            //     auto DinoChar = (APrimalDinoCharacter*)Cache.LocalActor;
            //     auto PrimalChar = (APrimalCharacter*)Cache.LocalActor;
             //    auto RiddenDino = Cache.LocalActor->GetBasedOrSeatingOnDino();

            //     if (LocalPlayer->PlayerController->WasInputKeyJustPressed(Settings.Keybinds.CharFreeze)) 
            //     { 
            //         Settings.Misc.NewSpeed = 0;
             //    }
             //    if (LocalPlayer->PlayerController->WasInputKeyJustPressed(Settings.Keybinds.SpeedOne))
            //     {
            //         Settings.Misc.NewSpeed = 1;
            //     }
            //     if (RiddenDino)
            //     {
            //         DinoChar->FlyingRunSpeedModifier = Settings.Misc.NewSpeed;
            //         DinoChar->RidingSwimmingRunSpeedModifier = Settings.Misc.NewSpeed;
            //         DinoChar->ScaleExtraRunningSpeedModifierSpeed = Settings.Misc.NewSpeed;
            //     }
                 //PrimalChar->ExtraMaxSpeedModifier = Settings.Misc.NewSpeed;
                 //PrimalChar->RunningSpeedModifier = Settings.Misc.NewSpeed;
                 //CharMove->Acceleration = Settings.Misc.NewSpeed;
                 //CharMove->CrouchedSpeedMultiplier = Settings.Misc.NewSpeed;
                 //CharMove->MaxFlySpeed = Settings.Misc.NewSpeed;
                 //CharMove->MaxSwimSpeed = Settings.Misc.NewSpeed;
                 //CharMove->MaxWalkSpeedCrouched = Settings.Misc.NewSpeed;
                 //CharMove->MaxWalkSpeedProne = Settings.Misc.NewSpeed;
            // }