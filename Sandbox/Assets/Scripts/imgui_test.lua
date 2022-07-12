-- ImGui test Lua Script for Razix Engine
-- This is a test for imgui scripting from lua, currently onle a few imgui elemts and types are available for use the API exposure needs to be improved later
-- Copyright Razix Engine 2022 by Pikachuxxxx (Phani Srikar)

function OnStart()
    RZLog.Info("ImGui Test!")
end

local value = false
local range = 0.0
local APIs = {"OpenGL", "Vulkan"}
local currentAPI = "OpenGL"
local isSelected = false

function get_value(res)
    value = res
    RZLog.Warn("isChecked : " .. tostring(res))
end

function get_dragFLoatVal(val)
    range = val
end

function get_isSelected(val)
    isSelected = val
end


function OnUpdate(dt)
    imgui.beginWindow("ImGui Lua Window")
        imgui.text("FPS : " .. (1 / dt))
        imgui.checkbox("Label", value, get_value)
        imgui.button("somebutton")
        imgui.dragFloat("Range", range, get_dragFLoatVal)

        if(imgui.beginCombo("Rendering API", currentAPI)) then
            for i = 1, 2, 1 do
                imgui.selectable(APIs[i], isSelected, get_isSelected)
                if(isSelected) then
                    currentAPI = APIs[i] 
                    RZLog.Info("SelectedAPI: " .. tostring(currentAPI))
                    imgui.setItemDefaultFocus()
                    isSelected = false
                    if (tostring(currentAPI) == "OpenGL") then
                        RZGraphicsContext.SetRenderAPI(0)
                    else 
                        RZGraphicsContext.SetRenderAPI(1)
                    end
                end
            end
            imgui.endCombo()
        end

    imgui.endWindow();
end
