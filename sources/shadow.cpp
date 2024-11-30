#include "shadow.hpp"

#include "manager.hpp"
#include "time.hpp"
#include "utilities.hpp"
#include "data.hpp"

#include <glm/gtx/transform2.hpp>
#include <glm/gtx/orthonormalize.hpp>

#include <stdexcept>
#include <algorithm>
#include <iostream>

void Shadow::Create()
{
	//GetShadowProjection(0);
	//GetShadowProjection(1);

	shadowCascadeViews.resize(cascadeCount);
	shadowCascadeProjections.resize(cascadeCount);
	shadowCascadeTransformations.resize(cascadeCount);

	CreateShadowPass();
	CreateShadowResources();
}

void Shadow::CreateShadowResources()
{
	//if (trapezoidal) CreateTrapezoidResources();
	CreateCascadeResources();
}

void Shadow::CreateTrapezoidResources()
{
	// if (shadowTexture.image != nullptr || shadowTexture.imageMemory != nullptr || shadowTexture.imageView != nullptr)
	//	throw std::runtime_error("cannot create shadow resources because they already exist");

	ImageConfiguration imageLod0Config;
	imageLod0Config.width = shadowLod0Resolution;
	imageLod0Config.height = shadowLod0Resolution;
	imageLod0Config.format = Manager::currentDevice.FindDepthFormat();
	imageLod0Config.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageLod0Config.aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
	imageLod0Config.transitionLayout = LAYOUT_READ_ONLY;

	ImageConfiguration imageLod1Config;
	imageLod1Config.width = shadowLod1Resolution;
	imageLod1Config.height = shadowLod1Resolution;
	imageLod1Config.format = Manager::currentDevice.FindDepthFormat();
	imageLod1Config.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageLod1Config.aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
	imageLod1Config.transitionLayout = LAYOUT_READ_ONLY;

	SamplerConfiguration samplerConfig;
	samplerConfig.repeatMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
	samplerConfig.borderColor = VK_BORDER_COLOR_INT_OPAQUE_WHITE;
	samplerConfig.anisotrophic = VK_FALSE;
	//samplerConfig.maxLod = 0.25f;
	//samplerConfig.magFilter = VK_FILTER_NEAREST;
	//samplerConfig.minFilter = VK_FILTER_NEAREST;

	shadowLod0Texture.CreateImage(imageLod0Config, samplerConfig);
	shadowLod1Texture.CreateImage(imageLod1Config, samplerConfig);
	shadowLod0Texture.TransitionImageLayout(imageLod0Config);
	shadowLod1Texture.TransitionImageLayout(imageLod1Config);

	VkFramebufferCreateInfo lod0FramebufferInfo{};
	lod0FramebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	lod0FramebufferInfo.renderPass = shadowTrapezoidPass;
	lod0FramebufferInfo.attachmentCount = 1;
	lod0FramebufferInfo.pAttachments = &shadowLod0Texture.imageView;
	lod0FramebufferInfo.width = shadowLod0Resolution;
	lod0FramebufferInfo.height = shadowLod0Resolution;
	lod0FramebufferInfo.layers = 1;

	if (vkCreateFramebuffer(Manager::currentDevice.logicalDevice, &lod0FramebufferInfo, nullptr, &shadowLod0FrameBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create shadow framebuffer");
	}

	VkFramebufferCreateInfo lod1FramebufferInfo{};
	lod1FramebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	lod1FramebufferInfo.renderPass = shadowTrapezoidPass;
	lod1FramebufferInfo.attachmentCount = 1;
	lod1FramebufferInfo.pAttachments = &shadowLod1Texture.imageView;
	lod1FramebufferInfo.width = shadowLod1Resolution;
	lod1FramebufferInfo.height = shadowLod1Resolution;
	lod1FramebufferInfo.layers = 1;

	if (vkCreateFramebuffer(Manager::currentDevice.logicalDevice, &lod1FramebufferInfo, nullptr, &shadowLod1FrameBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create shadow framebuffer");
	}
}

void Shadow::CreateCascadeResources()
{
	shadowCascadeTextures.resize(cascadeCount);
	shadowCascadeFrameBuffers.resize(cascadeCount);
	for (int i = 0; i < cascadeCount; i++)
	{
		ImageConfiguration imageConfig;
		imageConfig.width = shadowCascadeResolutions[i];
		imageConfig.height = shadowCascadeResolutions[i];
		imageConfig.format = Manager::currentDevice.FindDepthFormat();
		imageConfig.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageConfig.aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
		imageConfig.transitionLayout = LAYOUT_READ_ONLY;

		SamplerConfiguration samplerConfig;
		samplerConfig.repeatMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		samplerConfig.borderColor = VK_BORDER_COLOR_INT_OPAQUE_WHITE;
		samplerConfig.anisotrophic = VK_FALSE;

		shadowCascadeTextures[i].CreateImage(imageConfig, samplerConfig);

		VkFramebufferCreateInfo cascadeFramebufferInfo{};
		cascadeFramebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		cascadeFramebufferInfo.renderPass = shadowCascadePass;
		cascadeFramebufferInfo.attachmentCount = 1;
		cascadeFramebufferInfo.pAttachments = &shadowCascadeTextures[i].imageView;
		cascadeFramebufferInfo.width = shadowCascadeResolutions[i];
		cascadeFramebufferInfo.height = shadowCascadeResolutions[i];
		cascadeFramebufferInfo.layers = 1;

		if (vkCreateFramebuffer(Manager::currentDevice.logicalDevice, &cascadeFramebufferInfo, nullptr, &shadowCascadeFrameBuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create shadow cascade framebuffer");
		}
	}
}

void Shadow::CreateShadowPass()
{
	if (trapezoidal) CreateTrapezoidPass();
	if (!trapezoidal) CreateCascadePass();
}

void Shadow::CreateTrapezoidPass()
{
	VkAttachmentDescription depthAttachment{};
	depthAttachment.format = Manager::currentDevice.FindDepthFormat();
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = 0;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	std::vector<VkAttachmentDescription> attachments(1);
	attachments[0] = depthAttachment;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	// renderPassInfo.dependencyCount = 1;
	// renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(Manager::currentDevice.logicalDevice, &renderPassInfo, nullptr, &shadowTrapezoidPass) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create shadow pass");
	}
}

void Shadow::CreateCascadePass()
{
	VkAttachmentDescription depthAttachment{};
	depthAttachment.format = Manager::currentDevice.FindDepthFormat();
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = 0;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	std::vector<VkAttachmentDescription> attachments(1);
	attachments[0] = depthAttachment;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

	if (vkCreateRenderPass(Manager::currentDevice.logicalDevice, &renderPassInfo, nullptr, &shadowCascadePass) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create cascade shadow pass");
	}
}

void Shadow::Destroy()
{
	DestroyShadowPass();
	DestroyShadowResources();
}

void Shadow::DestroyShadowResources()
{
	if (shadowLod0FrameBuffer)
	{
		vkDestroyFramebuffer(Manager::currentDevice.logicalDevice, shadowLod0FrameBuffer, nullptr);
		shadowLod0FrameBuffer = nullptr;
	}
	if (shadowLod1FrameBuffer)
	{
		vkDestroyFramebuffer(Manager::currentDevice.logicalDevice, shadowLod1FrameBuffer, nullptr);
		shadowLod1FrameBuffer = nullptr;
	}

	shadowLod0Texture.Destroy();
	shadowLod1Texture.Destroy();

	for (VkFramebuffer &frameBuffer : shadowCascadeFrameBuffers)
	{
		vkDestroyFramebuffer(Manager::currentDevice.logicalDevice, frameBuffer, nullptr);
	}
	shadowCascadeFrameBuffers.clear();
	for (Texture &texture : shadowCascadeTextures)
	{
		texture.Destroy();
	}
	shadowCascadeTextures.clear();
}

void Shadow::DestroyShadowPass()
{
	if (shadowTrapezoidPass)
	{
		vkDestroyRenderPass(Manager::currentDevice.logicalDevice, shadowTrapezoidPass, nullptr);
		shadowTrapezoidPass = nullptr;
	}

	if (shadowCascadePass)
	{
		vkDestroyRenderPass(Manager::currentDevice.logicalDevice, shadowCascadePass, nullptr);
		shadowCascadePass = nullptr;
	}
}

glm::mat4 Shadow::GetTrapezoidView(int lod, float dis)
{
	glm::vec3 direction = Manager::shaderVariables.lightDirection;

	glm::vec3 focus = Manager::camera.Position();

	glm::vec3 front = glm::normalize(-direction);
	glm::vec3 side = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
	glm::vec3 up = glm::normalize(glm::cross(side, front));

	if (lod == 0)
	{
		glm::vec3 position = focus + direction * shadowCascadeDistances[lod] * dis;
		shadowLod0View = glm::lookAt(position, position + front, up);
		return (shadowLod0View);
	}
	else
	{
		glm::vec3 position = focus + direction * shadowLod1Distance * dis;
		shadowLod1View = glm::lookAt(position, position + front, up);
		return (shadowLod1View);
	}
}

void Shadow::SetCascadeViews()
{
	for (int i = 0; i < cascadeCount; i++)
	{
		glm::vec3 direction = Manager::shaderVariables.lightDirection;

		float near = 1.0f;
		for (int j = i; j > 0; j--) near += shadowCascadeDistances[j - 1];
		//if (i > 0) near = shadowCascadeDistances[i - 1];
		glm::vec3 focus = Manager::camera.Position() + Manager::camera.Front() * (near + shadowCascadeDistances[i]);
		//glm::vec3 focus = Manager::camera.Position();

		glm::vec3 front = glm::normalize(-direction);
		glm::vec3 side = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
		glm::vec3 up = glm::normalize(glm::cross(side, front));

		glm::vec3 position = focus + direction * shadowCascadeDistances[i] * 3.5f;
		shadowCascadeViews[i] = glm::lookAt(position, position + front, up);
	}
}

glm::mat4 Shadow::CreateBoundedProjection(int lod, float near, float far, float depthMult)
{
	std::vector<glm::vec4> frustumCorners = Manager::camera.GetFrustumCorners(near, near + far);

	std::vector<glm::vec4> frustumCornersInLightSpace;

	for (int i = 0; i < 8; i++)
	{
		frustumCornersInLightSpace.push_back(shadowCascadeViews[lod] * frustumCorners[i]);
	}

	glm::vec3 min(std::numeric_limits<float>::max());
	glm::vec3 max(std::numeric_limits<float>::lowest());

	for (const glm::vec4 &corner : frustumCornersInLightSpace)
	{
		glm::vec3 pos = glm::vec3(corner);
		min = glm::min(min, pos);
		max = glm::max(max, pos);
	}

	//float zNear = min.z;
	//float zFar = max.z;
	//min.z = 0.0f;
	//max.z = zFar - zNear;

	//glm::vec3 center = (min + max) * 0.5f;
	//float diff = glm::d
	//float offset = glm::max(max.x, max.y);

	return glm::ortho(min.x, max.x, min.y, max.y, 1.0f, far * depthMult);
	//return glm::ortho(min.x, max.x, min.y, max.y, 1.0f, far * depthMult);
	//return glm::ortho(min.x, max.x, min.y, max.y, 1.0f, max.z * depthMult);
}

std::vector<glm::vec2> GetBoundedPoints(std::vector<glm::vec2> points)
{
	std::vector<glm::vec2> result;
	result.resize(4);

	glm::vec2 min(std::numeric_limits<float>::max());
	glm::vec2 max(std::numeric_limits<float>::lowest());

	for (glm::vec2 corner : points)
	{
		min = glm::min(min, corner);
		max = glm::max(max, corner);
	}

	result[0] = glm::vec2(min.x, min.y);
	result[1] = glm::vec2(max.x, min.y);
	result[2] = glm::vec2(min.x, max.y);
	result[3] = glm::vec2(max.x, max.y);

	return (result);
}

float Cross(const Point2D &O, const Point2D &A, const Point2D &B)
{
	return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
}

std::vector<glm::vec2> ComputeConvexHull(std::vector<glm::vec2> convexPoints)
{
	if (convexPoints.size() <= 1) return convexPoints;

	std::vector<Point2D> points;
	for (const glm::vec2 &point : convexPoints)
	{
		points.push_back({point.x, point.y});
	}

	std::sort(points.begin(), points.end());

	std::vector<Point2D> hull;

	for (const auto &point : points)
	{
		while (hull.size() >= 2 && Cross(hull[hull.size() - 2], hull.back(), point) <= 0) hull.pop_back();

		hull.push_back(point);
	}

	size_t lowerSize = hull.size();
	for (auto it = points.rbegin(); it != points.rend(); ++it)
	{
		while (hull.size() > lowerSize && Cross(hull[hull.size() - 2], hull.back(), *it) <= 0) hull.pop_back();

		hull.push_back(*it);
	}

	hull.pop_back();

	//if (Time::newSecond)
	//{
	//	for (Point2D point : hull)
	//	{
	//		std::cout << point.x << "\t" << point.y << std::endl;
	//	}
	//	std::cout << std::endl;
	//}

	std::vector<glm::vec2> result;
	for (const Point2D &point : hull)
	{
		result.push_back(glm::vec2(point.x, point.y));
	}

	return result;
}

Line ComputeCenterLine(const std::vector<glm::vec2> &frustumCorners)
{
	glm::vec2 centerNear(0.0f, 0.0f), centerFar(0.0f, 0.0f);

	for (int i = 0; i < 4; i++)
	{
		centerNear += frustumCorners[i];
	}
	centerNear /= 4.0f;

	for (int i = 4; i < 8; i++)
	{
		centerFar += frustumCorners[i];
	}
	centerFar /= 4.0f;

	glm::vec2 direction = glm::normalize(centerFar - centerNear);

	return {(centerNear + centerFar) * 0.5f, direction};
}

std::pair<Line, Line> ComputeBaseAndTopLines(const std::vector<glm::vec2> &convexHull, const Line &centerLine)
{
	glm::vec2 basePoint, topPoint;

	float maxDistance = FLT_MAX, minDistance = FLT_MAX;
	glm::vec2 maxPoint = centerLine.point + centerLine.normal * 0.5f;
	glm::vec2 minPoint = centerLine.point - centerLine.normal * 0.5f;
	for (const glm::vec2 &p : convexHull)
	{
		glm::vec2 point(p.x, p.y);
		//float distance = glm::dot(point - centerLine.point, glm::vec2(-centerLine.normal.y, centerLine.normal.x));
		float distance = glm::distance(point, maxPoint);
		if (distance < maxDistance)
		{
			maxDistance = distance;
			basePoint = point;
		}
		distance = glm::distance(point, minPoint);
		if (distance < minDistance)
		{
			minDistance = distance;
			topPoint = point;
		}
	}

	glm::vec2 normal = glm::vec2(-centerLine.normal.y, centerLine.normal.x);

	//if (Time::newSecond)
	//{
	//	//Utilities::PrintVec("topPoint", topPoint);
	//	//Utilities::PrintVec("basePoint", basePoint);
	//	//Utilities::PrintVec("normal", normal);
	//	Utilities::PrintLine("topLine", topPoint, normal);
	//	Utilities::PrintLine("baseLine", basePoint, normal);
	//	std::cout << std::endl;
	//}

	return {{topPoint, normal}, {basePoint, normal}};
}

std::pair<Line, Line> ComputeSideLines(const std::vector<glm::vec2> &convexHull, const Line &centerLine, const glm::vec2 &q)
{
	glm::vec2 leftPoint, rightPoint;
	//glm::vec2 leftSecondPoint, rightSecondPoint;

	/*float maxLeftAngle = FLT_MAX;
	float maxRightAngle = -FLT_MAX;

	for (const glm::vec2 &point : convexHull)
	{
		glm::vec2 dir = glm::normalize(point - q);
		float angle = atan2(dir.y, dir.x);

		if (angle < maxLeftAngle)
		{
			maxLeftAngle = angle;
			leftPoint = point;
		}
		if (angle > maxRightAngle)
		{
			maxRightAngle = angle;
			rightPoint = point;
		}
	}*/

	//float rightMaxDistance = FLT_MAX, leftMaxDistance = FLT_MAX;
	//glm::vec2 normal = glm::vec2(-centerLine.normal.y, centerLine.normal.x);
	//glm::vec2 rightCheckPoint = centerLine.point + normal * 0.5f;
	//glm::vec2 leftCheckPoint = centerLine.point - normal * 0.5f;
	//for (const glm::vec2 &p : convexHull)
	//{
	//	glm::vec2 point(p.x, p.y);
	//	// float distance = glm::dot(point - centerLine.point, glm::vec2(-centerLine.normal.y, centerLine.normal.x));
	//	float distance = glm::distance(point, rightCheckPoint);
	//	if (distance < rightMaxDistance)
	//	{
	//		rightMaxDistance = distance;
	//		rightPoint = point;
	//	}
	//	distance = glm::distance(point, leftCheckPoint);
	//	if (distance < leftMaxDistance)
	//	{
	//		leftMaxDistance = distance;
	//		leftPoint = point;
	//	}
	//}

	float rightMaxAngle = -FLT_MAX, leftMaxAngle = FLT_MAX;
	//float rightSecondAngle = -FLT_MAX, leftSecondAngle = FLT_MAX;
	//glm::vec2 normal = glm::vec2(-centerLine.normal.y, centerLine.normal.x);
	//glm::vec2 rightCheckPoint = centerLine.point + normal * 0.5f;
	//glm::vec2 leftCheckPoint = centerLine.point - normal * 0.5f;
	float ca = glm::degrees(atan2(-centerLine.normal.y, centerLine.normal.x));
	float rightSpacing = 0;
	float leftSpacing = 0;
	for (const glm::vec2 &p : convexHull)
	{
		glm::vec2 point(p.x, p.y);
		// float distance = glm::dot(point - centerLine.point, glm::vec2(-centerLine.normal.y, centerLine.normal.x));
		//float distance = glm::distance(point, rightCheckPoint);
		glm::vec2 dir;
		if (abs(ca) <= 90.0f) dir = glm::normalize(point - q);
		else dir = glm::normalize(q - point);
		float angle = atan2(-dir.y, dir.x);
		float degrees = glm::degrees(angle);
		float spacing = ca - degrees;
		if (abs(ca) > 90.0f)
		{
			spacing -= 180.0f;
			if (ca < -90.0f) spacing += 360.0f;
		}
		spacing = abs(spacing);

		if (angle > rightMaxAngle)
		{
			//if (rightMaxAngle > rightSecondAngle)
			//{
			//	rightSecondAngle = rightMaxAngle;
			//	rightSecondPoint = rightPoint;
			//}

			rightMaxAngle = angle;
			rightPoint = point;
			rightSpacing = spacing;
		}
		//else if (angle > rightSecondAngle)
		//{
		//	rightSecondAngle = angle;
		//	rightSecondPoint = point;
		//}
		//distance = glm::distance(point, leftCheckPoint);
		if (angle < leftMaxAngle)
		{
			//if (leftMaxAngle < leftSecondAngle)
			//{
			//	leftSecondAngle = leftMaxAngle;
			//	leftSecondPoint = leftPoint;
			//}

			leftMaxAngle = angle;
			leftPoint = point;
			leftSpacing = spacing;
		}
		//else if (angle < leftSecondAngle)
		//{
		//	leftSecondAngle = angle;
		//	leftSecondPoint = point;
		//}
	}

	//if (rightSpacing > 62.0f) rightPoint = rightSecondPoint;
	//if (leftSpacing > 62.0f) leftPoint = leftSecondPoint;

	//float startSpacing = 100.0f;
	//float endSpacing = 120.0f;
	//float diffSpacing = endSpacing - startSpacing;
	//if (rightSpacing + leftSpacing > startSpacing)
	//{
	//	float iter = glm::clamp((rightSpacing + leftSpacing) - startSpacing, 0.0f, diffSpacing) / diffSpacing;
	//	rightPoint = glm::mix(rightPoint, rightSecondPoint, iter);
	//	leftPoint = glm::mix(leftPoint, leftSecondPoint, iter);
	//}

	//if (Time::newSecond)
	//{
	//	std::cout << "ca: " << ca << std::endl;
	//	std::cout << "ra: " << glm::degrees(rightMaxAngle) << std::endl;
	//	std::cout << "la: " << glm::degrees(leftMaxAngle) << std::endl;
	//	std::cout << "cra: " << rightSpacing << std::endl;
	//	std::cout << "cla: " << leftSpacing << std::endl << std::endl;
	//	std::cout << "sva: " << abs(glm::dot(Manager::shaderVariables.lightDirection, Manager::camera.Front())) << std::endl << std::endl;
	//}

	glm::vec2 normalLeft = glm::normalize(leftPoint - q);
	glm::vec2 normalRight = glm::normalize(rightPoint - q);

	return {{leftPoint, normalLeft}, {rightPoint, normalRight}};
}

glm::vec2 IntersectLines(Line line1, Line line2)
{
	//glm::mat2 A(glm::vec2(line1.normal), glm::vec2(line2.normal));
	//glm::vec2 b(glm::dot(line1.point, line1.normal), glm::dot(line2.point, line2.normal));

	//return (glm::inverse(A) * b);

	//line1.normal = glm::vec2(line1.normal.y, -line1.normal.x);
	//line2.normal = glm::vec2(-line2.normal.y, line2.normal.x);
	//glm::mat2 A = glm::mat2(line1.normal, line2.normal);

	//float det = glm::determinant(A);
	//if (glm::abs(det) < 1e-6f)
	//{
	//	return glm::vec2(0);
	//}

	//glm::vec2 b = glm::vec2(glm::dot(line1.normal, line1.point), glm::dot(line2.normal, line2.point));

	//glm::vec2 intersection = glm::inverse(A) * b;
	//return intersection;

	glm::vec2 p1 = line1.point;
	//glm::vec2 n1 = line1.normal;
	glm::vec2 n1 = glm::vec2(-line1.normal.y, line1.normal.x);
	glm::vec2 p2 = line2.point;
	//glm::vec2 n2 = line2.normal;
	glm::vec2 n2 = glm::vec2(-line2.normal.y, line2.normal.x);

	// Compute constants for the line equations
	float c1 = glm::dot(n1, p1); // n1.x * p1.x + n1.y * p1.y
	float c2 = glm::dot(n2, p2); // n2.x * p2.x + n2.y * p2.y

	// Matrix determinant
	float det = n1.x * n2.y - n1.y * n2.x;

	// Check if determinant is zero (parallel lines)
	if (glm::abs(det) < 1e-6f)
	{
		return glm::vec2(0); // Lines are parallel or coincident
	}

	// Solve using Cramer's rule
	float x = (c1 * n2.y - c2 * n1.y) / det;
	float y = (n1.x * c2 - n2.x * c1) / det;

	return glm::vec2(x, y);
}

Trapezoid ComputeTrapezoid(const std::vector<glm::vec2> &convexHull, const Line &centerLine, float focusRatio)
{
	//Line centerLine = ComputeCenterLine(convexHull);

	//auto [topLine, baseLine] = ComputeBaseAndTopLines(convexHull, centerLine);

	//auto [leftLine, rightLine] = ComputeSideLines(convexHull, focusRatio);

	//glm::vec2 topLeft = IntersectLines(topLine, leftLine);
	//glm::vec2 topRight = IntersectLines(topLine, rightLine);
	//glm::vec2 bottomLeft = IntersectLines(baseLine, leftLine);
	//glm::vec2 bottomRight = IntersectLines(baseLine, rightLine);

	//if (Time::newSecond)
	//{
	//	std::cout << "centerLine: " << centerLine.normal.x << " | " << centerLine.normal.y << std::endl;
	//	std::cout << "topLeft: " << topLeft.x << " | " << topLeft.y << std::endl;
	//	std::cout << "topRight: " << topRight.x << " | " << topRight.y << std::endl;
	//	std::cout << "bottomLeft: " << bottomLeft.x << " | " << bottomLeft.y << std::endl;
	//	std::cout << "bottomRight: " << bottomRight.x << " | " << bottomRight.y << std::endl << std::endl;
	//}

	//return {topLeft, topRight, bottomLeft, bottomRight};
	return {};
}

glm::vec2 TransformPoint(const glm::vec2 &point, const glm::mat4 &translate, const glm::mat4 &rotate)
{
	glm::vec4 homogenousPoint(point.x, point.y, 0.0f, 1.0f);
	glm::vec4 transformedPoint = rotate * translate * homogenousPoint;

	return glm::vec2(transformedPoint.x, transformedPoint.y);
}

glm::mat4 ComputeTrapezoidalMatrix(const Trapezoid &trapezoid, const glm::vec2 &q, bool useQ)
{
	/*glm::mat4 translateToOrigin = glm::translate(glm::mat4(1.0f), glm::vec3(-trapezoid.bottomLeft.x, -trapezoid.bottomLeft.y, 0.0f));

	float angle = -atan2(trapezoid.bottomRight.y - trapezoid.bottomLeft.y, trapezoid.bottomRight.x - trapezoid.bottomLeft.x);
	glm::mat4 rotateToAlign = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 0.0f, 1.0f));

	glm::vec2 topLeftT = TransformPoint(trapezoid.topLeft, translateToOrigin, rotateToAlign);
	glm::vec2 topRightT = TransformPoint(trapezoid.topRight, translateToOrigin, rotateToAlign);
	glm::vec2 bottomLeftT = TransformPoint(trapezoid.bottomLeft, translateToOrigin, rotateToAlign);
	glm::vec2 bottomRightT = TransformPoint(trapezoid.bottomRight, translateToOrigin, rotateToAlign);

	float height = topLeftT.y - bottomLeftT.y;
	float widthTop = topRightT.x - topLeftT.x;

	glm::mat4 scaleToUnitHeight = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f / height, 1.0f));
	glm::mat4 scaleToUnitWidth = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f / widthTop, 1.0f, 1.0f));

	return (scaleToUnitWidth * scaleToUnitHeight * rotateToAlign * translateToOrigin);*/

	//glm::vec2 t0 = trapezoid.bottomLeft * 0.5f + 0.5f;
	//glm::vec2 t1 = trapezoid.bottomRight * 0.5f + 0.5f;
	//glm::vec2 t2 = trapezoid.topRight * 0.5f + 0.5f;
	//glm::vec2 t3 = trapezoid.topLeft * 0.5f + 0.5f;
	glm::vec4 t0 = glm::vec4(trapezoid.bottomLeft, 0.0, 1.0);
	glm::vec4 t1 = glm::vec4(trapezoid.bottomRight, 0.0, 1.0);
	glm::vec4 t2 = glm::vec4(trapezoid.topRight, 0.0, 1.0);
	glm::vec4 t3 = glm::vec4(trapezoid.topLeft, 0.0, 1.0);

	//glm::mat4 NT = glm::mat4(1.0f);

	if (!useQ)
	{
		glm::vec2 u = (trapezoid.bottomLeft + trapezoid.bottomRight + trapezoid.topRight + trapezoid.topLeft) / 4.0f;

		glm::mat4 T1 = glm::translate(glm::mat4(1.0f), glm::vec3(-u.x, -u.y, 0));

		/*glm::vec4 uMax1 = glm::max(glm::abs(T1 * t0), glm::abs(T1 * t1));
		glm::vec4 uMax2 = glm::max(glm::abs(T1 * t2), glm::abs(T1 * t3));
		glm::vec4 uMax0 = glm::max(uMax1, uMax2);

		glm::mat4 S1 = glm::scale(glm::mat4(1.0f), glm::vec3(1.0 / uMax0.x, 1.0 / uMax0.y, 1.0));

		glm::mat4 S2 = glm::scale(glm::mat4(1.0f), glm::vec3(0.5, 0.5, 1.0));

		glm::mat4 T2 = glm::translate(glm::mat4(1.0f), glm::vec3(0.5, 0.5, 0));

		if (Time::newSecond)
		{
			u = T1 * t3;
			Utilities::PrintVec(u);
			u = S1 * T1 * t3;
			Utilities::PrintVec(u);
			std::cout << std::endl;
		}*/

		//glm::mat4 T1 = glm::mat4(1.0f);

		//u = glm::normalize(t2 - t3);
		//float angle = atan2(-u.y, u.x);

		//u = T1 * t2;

		//glm::mat4 S1 = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f / u.x, 1.0f / u.y, 1.0));

		//glm::mat4 N = glm::mat4(
		//	1, 0, 0, 0,
		//	0, 1, 0, 1,
		//	0, 0, 1, 0,
		//	0, 1, 0, 0);

		//u = T1 * t0;
		//glm::vec4 v = T1 * t2;

		//glm::mat4 T3 = glm::translate(glm::mat4(1.0f), glm::vec3(0, -(u.y / u.w + v.y / v.w) / 2.0f, 0));

		//glm::mat4 R = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 0, 1));

		//u = T3 * S1 * T1 * t0;

		//glm::mat4 S2 = glm::scale(glm::mat4(1.0f), glm::vec3(1, -u.w / u.y, 1));

		//return (S2 * T3 * S1 * T1);



		return (T1);
	}

	//glm::vec4 center = (t2 + t3) / 2.0f;
	//glm::vec4 centerBase = (t0 + t1) / 2.0f;

	glm::vec4 u = (t2 + t3) / 2.0f;

	glm::mat4 T1 = glm::translate(glm::mat4(1.0f), glm::vec3(-u.x, -u.y, 0));

	u = glm::normalize(t2 - t3);
	float angle = atan2(-u.y, u.x);

	glm::mat4 R = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 0, 1));

	u = R * T1 * glm::vec4(q, 0.0, 1.0);

	glm::mat4 T2 = glm::translate(glm::mat4(1.0f), glm::vec3(-u.x, -u.y, 0));

	u = (T2 * R * T1 * (t2 + t3)) / 2.0f;

	glm::mat4 H = glm::shearX2D(glm::mat3(1.0f), -u.x / u.y);

	u = H * T2 * R * T1 * t2;

	glm::mat4 S1 = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f/u.x, 1.0f/u.y, 1.0));

	glm::mat4 N = glm::mat4(
		1, 0, 0, 0,
		0, 1, 0, 1,
		0, 0, 1, 0,
		0, 1, 0, 0);

	u = N * S1 * H * T2 * R * T1 * t0;
	glm::vec4 v = N * S1 * H * T2 * R * T1 * t2;

	glm::mat4 T3 = glm::translate(glm::mat4(1.0f), glm::vec3(0, -(u.y / u.w + v.y / v.w) / 2.0f, 0));

	u = T3 * N * S1 * H * T2 * R * T1 * t0;

	glm::mat4 S2 = glm::scale(glm::mat4(1.0f), glm::vec3(1, -u.w / u.y, 1));

	return (S2 * T3 * N * S1 * H * T2 * R * T1);
	//return (R * T1);
}

glm::mat4 Shadow::GetTrapezoidProjection(int lod)
{
	if (lod == 0)
	{
		//shadowLod0Projection = CreateBoundedProjection(GetShadowView(0), shadowLod0Distance);
		//return (shadowLod0Projection);

		//shadowLod0Projection = glm::ortho(-shadowLod0Distance, shadowLod0Distance, -shadowLod0Distance, 
		//	shadowLod0Distance, 1.0f, shadowLod0Distance * 2.0f);
		//shadowLod0Projection[1][1] *= -1;
		//return (shadowLod0Projection);

		shadowLod0Projection = glm::ortho(-shadowCascadeDistances[lod], shadowCascadeDistances[lod], -shadowCascadeDistances[lod], 
			shadowCascadeDistances[lod], 1.0f, shadowCascadeDistances[lod] * 6.0f);
		shadowLod0Projection[1][1] *= -1;
		return (shadowLod0Projection);

		// shadowLod0Projection = glm::ortho(-shadowLod0Distance, shadowLod0Distance, -shadowLod0Distance, shadowLod0Distance, 1.0f, shadowLod0Distance * 2.0f);
		// shadowLod0Projection[1][1] *= -1;
		// return (shadowLod0Projection);
	}
	else
	{
		//shadowLod1Projection = CreateBoundedProjection(GetShadowView(1), shadowLod1Distance);
		//return (shadowLod1Projection);

		shadowLod1Projection = glm::ortho(-shadowLod1Distance, shadowLod1Distance, -shadowLod1Distance, 
			shadowLod1Distance, 1.0f, shadowLod1Distance * 2.0f);
		shadowLod1Projection[1][1] *= -1;
		return (shadowLod1Projection);

		//shadowLod1Projection = glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, shadowLod1Distance * 2);
		//return (shadowLod1Projection);
	}
}

void Shadow::SetCascadeProjections()
{
	for (int i = 0; i < cascadeCount; i++)
	{
		float near = 1.0f;
		if (i > 0) near = shadowCascadeDistances[i - 1];

		shadowCascadeProjections[i] = glm::ortho(-shadowCascadeDistances[i], shadowCascadeDistances[i], -shadowCascadeDistances[i], shadowCascadeDistances[i], 1.0f, shadowCascadeDistances[i] * 6.0f);
		shadowCascadeProjections[i][1][1] *= -1;

		//shadowCascadeProjections[i] = CreateBoundedProjection(i, near, shadowCascadeDistances[i], 2.0f);
		//shadowCascadeProjections[i][1][1] *= -1;
	}
}

glm::vec2 Shadow::ComputeQ(const Line &centerLine, const Line &topLine, float delta, int lod, float far)
{
	//glm::vec3 worldPos = Manager::camera.Position() + Manager::camera.Front() * 2.5f;
	//glm::vec3 worldPos = Manager::camera.Position() + Manager::camera.Front() * 100.0f;

	glm::vec3 worldPos;
	glm::vec4 shadowPos;

	if (lod == 0)
	{
		worldPos = Manager::camera.Position() + Manager::camera.Front() * (far * 0.4f);
		shadowPos = shadowLod0Projection * shadowLod0View * glm::vec4(worldPos, 1.0f);
	}
	else if (lod == 1)
	{
		//worldPos = Manager::camera.Position() + Manager::camera.Front() * (shadowLod0Distance * 0.5f + (shadowLod1Distance * 0.2f));
		worldPos = Manager::camera.Position() + Manager::camera.Front() * (far * 0.4f);
		shadowPos = shadowLod1Projection * shadowLod1View * glm::vec4(worldPos, 1.0f);
	}

	//glm::vec4 shadowPos = shadowLod1View * glm::vec4(worldPos, 1.0f);
	//glm::vec2 trapezoidPos = (glm::vec2(shadowPos.x, shadowPos.y) / shadowPos.w) * 0.5f + 0.5f;
	//glm::vec2 trapezoidPos = (glm::vec2(shadowPos.x, shadowPos.y) / shadowPos.w);
	glm::vec2 trapezoidPos = glm::vec2(shadowPos.x, shadowPos.y);

	float disToTop = glm::distance(topLine.point, trapezoidPos);
	float mapY = -0.6f;

	float qDis = ((delta * disToTop) + (delta * disToTop * mapY)) / (delta - (2.0f * disToTop) - (delta * mapY));

	glm::vec2 q = trapezoidPos - centerLine.normal * (qDis + disToTop);

	//if (Time::newSecond)
	//{
	//	Utilities::PrintPoint("closePoint", trapezoidPos);
	//	Utilities::PrintPoint("q", q);
	//	std::cout << std::endl;
	//}

	//return glm::vec3(q, qDis);
	return (q);
}

void Shadow::SetCascadeTransformations()
{
	for (int i = 0; i < cascadeCount; i++)
	{
		/*float near = 1.0f;
		if (i > 0) near = shadowCascadeDistances[i - 1];
		std::vector<glm::vec4> frustumCorners = Manager::camera.GetFrustumCorners(near, shadowCascadeDistances[i]);
		std::vector<glm::vec2> frustumCornersInLightSpace;
		for (const glm::vec4 &corner : frustumCorners) frustumCornersInLightSpace.push_back(shadowCascadeProjections[i] * shadowCascadeViews[i] * corner);

		std::vector<glm::vec2> farPoints;
		farPoints.push_back(frustumCornersInLightSpace[4]);
		farPoints.push_back(frustumCornersInLightSpace[5]);
		farPoints.push_back(frustumCornersInLightSpace[6]);
		farPoints.push_back(frustumCornersInLightSpace[7]);
		std::vector<glm::vec2> boundedPoints = GetBoundedPoints(farPoints);

		glm::vec2 u = (boundedPoints[0] + boundedPoints[1] + boundedPoints[2] + boundedPoints[3]) / 4.0f;

		glm::mat4 T1 = glm::translate(glm::mat4(1.0f), glm::vec3(-u.x, -u.y, 0));*/

		glm::mat4 R = glm::rotate(glm::mat4(1.0f), glm::radians(-Manager::camera.Angles().y + 45.0f), glm::vec3(0, 0, 1));

		shadowCascadeTransformations[i] = R;
	}
}

glm::mat4 Shadow::GetTrapezoidTransformation(int lod)
{
	GetTrapezoidView(lod, 3.0f);
	GetTrapezoidProjection(lod);

	//float maxDepth = Data::GetData().frustumIntersect;
	float maxDepth = 0;
	float frustumFar = 0;
	std::vector<glm::vec4> frustumCorners = Manager::camera.GetFrustumCorners(1.0f, shadowCascadeDistances[lod]);
	//if (lod == 0)
	//{
	//	//frustumFar = glm::min(shadowLod0Distance * 0.5f, glm::clamp(maxDepth, shadowLod0Distance * 0.375f, shadowLod0Distance));
	//	frustumFar = shadowLod0Distance * 0.5f;
	//	frustumCorners = Manager::camera.GetFrustumCorners(1.0f, frustumFar);
	//}
	//else if (lod == 1)
	//{
	//	//frustumCorners = Manager::camera.GetFrustumCorners(shadowLod0Distance * 0.5f, shadowLod1Distance * 0.5f);
	//	//frustumFar = glm::min(shadowLod1Distance * 0.5f, glm::clamp(maxDepth, shadowLod1Distance * 0.25f, shadowLod1Distance));
	//	frustumFar = shadowLod1Distance * 0.5f;
	//	frustumCorners = Manager::camera.GetFrustumCorners(1.0f, frustumFar);
	//}

	//std::vector<Point2D> frustumCorners2D;
	//for (glm::vec4 point : frustumCorners)
	//{
	//	glm::vec4 transformedPoint = shadowLod1Projection * shadowLod1View * point;
	//	transformedPoint /= transformedPoint.w;
	//	frustumCorners2D.push_back(Point2D{transformedPoint.x, transformedPoint.y});
	//}
	//std::vector<Point2D> convexHull = ComputeConvexHull(frustumCorners2D);
	//Trapezoid trapezoid = ComputeTrapezoid(convexHull, 0.8f);
	//shadowLod1Transformation = ComputeTrapezoidalMatrix(trapezoid);

	std::vector<glm::vec2> frustumCornersInLightSpace;
	for (const glm::vec4 &corner : frustumCorners)
	{
		//glm::vec4 lightSpaceCorner = shadowLod1View * corner;
		//glm::vec2 postProjectionCorner = glm::vec2(lightSpaceCorner.x, lightSpaceCorner.y) / lightSpaceCorner.w;
		//glm::vec2 postProjectionCorner = (glm::vec2(lightSpaceCorner.x, lightSpaceCorner.y) / lightSpaceCorner.w) * 0.5f + 0.5f;

		//glm::vec4 lightSpaceCorner;
		//if (lod == 0) lightSpaceCorner = shadowLod0Projection * shadowLod0View * corner;
		//else if (lod == 1) lightSpaceCorner = shadowLod1Projection * shadowLod1View * corner;

		//glm::vec2 postProjectionCorner = glm::vec2(lightSpaceCorner.x, lightSpaceCorner.y);
		frustumCornersInLightSpace.push_back(shadowCascadeProjections[lod] * shadowCascadeViews[lod] * corner);
	}

	//for (int i = 0; i < 8; i++)
	//{
	//	Manager::shaderVariables.frustumCorners[i] = frustumCornersInLightSpace[i];
	//}

	Line centerLine = ComputeCenterLine(frustumCornersInLightSpace);

	std::vector<glm::vec2> convexHull = ComputeConvexHull(frustumCornersInLightSpace);

	float dualFrusta = abs(glm::dot(Manager::shaderVariables.lightDirection, Manager::camera.Front()));

	//if (Time::newSecond)
	//{
	//	std::cout << dualFrusta << std::endl;
	//}

	//if (dualFrusta > 0.75f)
	//if (convexHull.size() == 4)
	{
		//if (Time::newSecond)
		//{
		//	std::cout << Data::GetData().frustumIntersectAverage << std::endl;
		//}

		//Trapezoid trapezoid = {convexHull[0], convexHull[1], convexHull[2], convexHull[3]};
		//glm::vec2 q = (convexHull[0] + convexHull[1]) * 0.5f;

		Manager::shaderVariables.shadowBounding = 1;

		if (lod == 0)
		{
			//shadowLod0Transformation = ComputeTrapezoidalMatrix(trapezoid, q);
			float range = 0.2f;
			//float height = Manager::camera.Position().y - Data::GetData().viewHeight * 5000.0f;
			//range = glm::clamp(height / 50.0f, 0.01f, 1.0f);
			//float depth = Data::GetData().frustumIntersectAverage;
			//range = glm::clamp(depth / 100.0f, 0.01f, 1.0f);

			//float depth = Data::GetData().frustumIntersectAverage;
			//float depth = 0;
			//range = glm::clamp(depth / (shadowLod0Distance * 0.5f), 0.01f, 0.2f);
			//shadowLod0View = GetTrapezoidView(0, range);
			//shadowLod0Projection = CreateBoundedProjection(shadowLod0View, 1.0f, shadowLod0Distance * range, false);
			//shadowLod0Transformation = glm::mat4(1.0f);
			//if (Time::newSecond)
			//{
			//	for (glm::vec2 point : convexHull) Utilities::PrintVec(point);
			//	std::cout << std::endl;
			//} //t3 t2 t0 t1
			std::vector<glm::vec2> farPoints;
			farPoints.push_back(frustumCornersInLightSpace[4]);
			farPoints.push_back(frustumCornersInLightSpace[5]);
			farPoints.push_back(frustumCornersInLightSpace[6]);
			farPoints.push_back(frustumCornersInLightSpace[7]);
			std::vector<glm::vec2> boundedPoints = GetBoundedPoints(farPoints);
			Trapezoid trapezoid = {boundedPoints[2], boundedPoints[3], boundedPoints[0], boundedPoints[1]};
			Manager::shaderVariables.frustumCorner1 = glm::vec3(trapezoid.topLeft, 0.0f);
			Manager::shaderVariables.frustumCorner2 = glm::vec3(trapezoid.topRight, 0.0f);
			Manager::shaderVariables.frustumCorner3 = glm::vec3(trapezoid.bottomLeft, 0.0f);
			Manager::shaderVariables.frustumCorner4 = glm::vec3(trapezoid.bottomRight, 0.0f);
			shadowLod0Transformation = ComputeTrapezoidalMatrix(trapezoid, glm::vec2(0), false);
			return (shadowLod0Transformation);
		}
		else if (lod == 1)
		{
			//shadowLod1Transformation = ComputeTrapezoidalMatrix(trapezoid, q);
			float range = 1.0f;
			//float height = Manager::camera.Position().y - Data::GetData().viewHeight * 5000.0f;
			//float heightRange = glm::clamp(height / 100.0f, 0.05f, 1.0f);
			//heightRange = 1.0f - pow(1.0f - heightRange, 2.0f);

			// float depth = Data::GetData().frustumIntersectAverage;
			float depth = 0;
			float depthRange = glm::clamp(depth / (shadowLod1Distance * 0.5f), 0.1f, 1.0f);
			//range = glm::min(heightRange, depthRange);
			range = depthRange;
			shadowLod1View = GetTrapezoidView(1, range);
			//shadowLod1Projection = CreateBoundedProjection(shadowLod1View, 1.0f, shadowLod1Distance * range, false);
			shadowLod1Transformation = glm::mat4(1.0f);
			return (shadowLod1Transformation);
		}
	}
	Manager::shaderVariables.shadowBounding = 0;

	auto [topLine, baseLine] = ComputeBaseAndTopLines(convexHull, centerLine);

	float delta = glm::distance(topLine.point, baseLine.point);
	// glm::vec3 qResult = ComputeQ(centerLine, topLine, delta);
	// glm::vec2 q = glm::vec2(qResult.x, qResult.y);
	glm::vec2 q = ComputeQ(centerLine, topLine, delta, lod, frustumFar);

	auto [leftLine, rightLine] = ComputeSideLines(convexHull, centerLine, q);

	glm::vec2 topLeft = IntersectLines(leftLine, topLine);
	glm::vec2 topRight = IntersectLines(rightLine, topLine);
	glm::vec2 baseLeft = IntersectLines(leftLine, baseLine);
	glm::vec2 baseRight = IntersectLines(rightLine, baseLine);

	Trapezoid trapezoid = {topLeft, topRight, baseLeft, baseRight};

	Manager::shaderVariables.frustumCorner1 = glm::vec3(topLeft, 0.0f);
	Manager::shaderVariables.frustumCorner2 = glm::vec3(topRight, 0.0f);
	Manager::shaderVariables.frustumCorner3 = glm::vec3(baseLeft, 0.0f);
	Manager::shaderVariables.frustumCorner4 = glm::vec3(baseRight, 0.0f);

	//ComputeTrapezoid(convexHull, l, 0.8f);

	bool logTrapezoid = false;
	if (logTrapezoid && Time::newSecond)
	{
		std::cout << "near plane: " << std::endl;
		for (int i = 0; i < 4; i++)
		{
			//std::cout << frustumCornersInLightSpace[i].x << "\t" << frustumCornersInLightSpace[i].y << std::endl;
			Utilities::PrintPoint(frustumCornersInLightSpace[i]);
		}
		//std::cout << frustumCornersInLightSpace[0].x << "\t" << frustumCornersInLightSpace[0].y << std::endl;
		Utilities::PrintPoint(frustumCornersInLightSpace[0]);
		std::cout << std::endl;

		std::cout << "far plane: " << std::endl;
		for (int i = 4; i < 8; i++)
		{
			//std::cout << frustumCornersInLightSpace[i].x << "\t" << frustumCornersInLightSpace[i].y << std::endl;
			Utilities::PrintPoint(frustumCornersInLightSpace[i]);
		}
		//std::cout << frustumCornersInLightSpace[4].x << "\t" << frustumCornersInLightSpace[4].y << std::endl;
		Utilities::PrintPoint(frustumCornersInLightSpace[4]);
		std::cout << std::endl;

		std::cout << "frustum lines: " << std::endl;
		for (int i = 0; i < 4; i++)
		{
			//std::cout << frustumCornersInLightSpace[i].x << "\t" << frustumCornersInLightSpace[i].y << std::endl;
			//std::cout << frustumCornersInLightSpace[i + 4].x << "\t" << frustumCornersInLightSpace[i + 4].y << std::endl;
			Utilities::PrintPoint(frustumCornersInLightSpace[i]);
			Utilities::PrintPoint(frustumCornersInLightSpace[i + 4]);
			std::cout << std::endl;
		}

		std::cout << "convex hull: " << std::endl;
		for (int i = 0; i < convexHull.size(); i++)
		{
			//std::cout << convexHull[i].x << "\t" << convexHull[i].y << std::endl;
			Utilities::PrintPoint(convexHull[i]);
		}
		//std::cout << convexHull[0].x << "\t" << convexHull[0].y << std::endl;
		Utilities::PrintPoint(convexHull[0]);
		std::cout << std::endl;

		Utilities::PrintLine("", centerLine.point, centerLine.normal);
		Utilities::PrintLine("", topLine.point, topLine.normal);
		Utilities::PrintLine("", baseLine.point, baseLine.normal);
		Utilities::PrintLine("", leftLine.point, leftLine.normal);
		Utilities::PrintLine("", rightLine.point, rightLine.normal);
		std::cout << std::endl;

		//Utilities::PrintPoint("topLeft", IntersectLines(leftLine, topLine));
		//Utilities::PrintPoint("topRight", IntersectLines(rightLine, topLine));
		//Utilities::PrintPoint("baseLeft", IntersectLines(leftLine, baseLine));
		//Utilities::PrintPoint("baseRight", IntersectLines(rightLine, baseLine));
		std::cout << "points: " << std::endl;
		//std::cout << q.x << "\t" << q.y << std::endl;
		//std::cout << topLeft.x << "\t" << topLeft.y << std::endl;
		//std::cout << topRight.x << "\t" << topRight.y << std::endl;
		//std::cout << baseLeft.x << "\t" << baseLeft.y << std::endl;
		//std::cout << baseRight.x << "\t" << baseRight.y << std::endl;
		Utilities::PrintPoint(q);
		Utilities::PrintPoint(topLeft);
		Utilities::PrintPoint(topRight);
		Utilities::PrintPoint(baseLeft);
		Utilities::PrintPoint(baseRight);
		std::cout << std::endl;
	}

	if (lod == 0)
	{
		shadowLod0Transformation = ComputeTrapezoidalMatrix(trapezoid, q, true);
		//shadowLod0Transformation[1][1] *= -1;
		return (shadowLod0Transformation);
	}
	else if (lod == 1)
	{
		shadowLod1Transformation = ComputeTrapezoidalMatrix(trapezoid, q, true);
		//shadowLod1Transformation[1][1] *= -1;
		return (shadowLod1Transformation);
	}
	
	return (glm::mat4(1.0f));
}

int Shadow::cascadeCount = 4;
VkRenderPass Shadow::shadowCascadePass = nullptr;
std::vector<VkFramebuffer> Shadow::shadowCascadeFrameBuffers;
std::vector<Texture> Shadow::shadowCascadeTextures;
std::vector<glm::mat4> Shadow::shadowCascadeViews;
std::vector<glm::mat4> Shadow::shadowCascadeProjections;
std::vector<glm::mat4> Shadow::shadowCascadeTransformations;
std::vector<float> Shadow::shadowCascadeDistances = {25, 75, 200, 200};
std::vector<int> Shadow::shadowCascadeResolutions = {4096, 4096, 2048, 2048};

bool Shadow::trapezoidal = false;

VkRenderPass Shadow::shadowTrapezoidPass = nullptr;
VkFramebuffer Shadow::shadowLod0FrameBuffer = nullptr;
VkFramebuffer Shadow::shadowLod1FrameBuffer = nullptr;
Texture Shadow::shadowLod0Texture;
Texture Shadow::shadowLod1Texture;

glm::mat4 Shadow::shadowLod0View = glm::mat4(1);
glm::mat4 Shadow::shadowLod0Projection = glm::mat4(1);
glm::mat4 Shadow::shadowLod0Transformation = glm::mat4(1);
glm::mat4 Shadow::shadowLod1View = glm::mat4(1);
glm::mat4 Shadow::shadowLod1Projection = glm::mat4(1);
glm::mat4 Shadow::shadowLod1Transformation = glm::mat4(1);

int Shadow::shadowLod0Resolution = 4096;
float Shadow::shadowLod0Distance = 50;
int Shadow::shadowLod1Resolution = 4096;
float Shadow::shadowLod1Distance = 1000;